/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <algorithm>
#include <exception>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <glbinding/Binding.h>
#include <glbinding/gl33core/gl.h>
#include <glbinding/glbinding.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

#include <mata/concepts/noncopyable.hpp>
#include <mata/layer.hpp>
#include <mata/renderer.hpp>
#include <mata/virtual-file-system.hpp>

using namespace gl;

namespace mata {

typedef GLuint shaderprogram_h;
typedef GLuint shader_h;
typedef GLuint buffer_h;

inline std::string readableErrorCode(const gl::GLenum errorCode) {
  if (GL_INVALID_ENUM == errorCode) {
    return "GL_INVALID_ENUM";
  }
  if (gl::GL_INVALID_VALUE == errorCode) {
    return "GL_INVALID_VALUE";
  }
  if (gl::GL_INVALID_OPERATION == errorCode) {
    return "GL_INVALID_OPERATION";
  }
  if (gl::GL_INVALID_FRAMEBUFFER_OPERATION == errorCode) {
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  }
  return "Unknown Error";
}

struct LayerH {
  buffer_h vao;
  unsigned int nIndices;
};

class [[nodiscard]] Renderer::Impl final : private noncopyable {
  std::shared_ptr<VirtualFileSystem> m_pVfs;
  shaderprogram_h m_hShaderProgram{0};
  bool m_wireframeModeEnabled = false;
  std::vector<LayerH> m_layers{};

  void clearScreen() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(ClearBufferMask::GL_COLOR_BUFFER_BIT);
  }

  [[nodiscard]] shaderprogram_h initShaderProgram() {
    const auto hVertexShader =
        this->loadShader("default.vert", GL_VERTEX_SHADER);
    const auto hFragmentShader =
        this->loadShader("default.frag", GL_FRAGMENT_SHADER);
    const auto hShaderProgram = glCreateProgram();
    glAttachShader(hShaderProgram, hVertexShader);
    glAttachShader(hShaderProgram, hFragmentShader);
    glLinkProgram(hShaderProgram);
    int success;
    glGetProgramiv(hShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetProgramInfoLog(hShaderProgram, 512, nullptr, infoLog);
      throw std::runtime_error(fmt::format("Failed to link shader program: {0}",
                                           std::string_view(infoLog)));
    }
    glDeleteShader(hFragmentShader);
    glDeleteShader(hVertexShader);
    return hShaderProgram;
  }

  [[nodiscard]] shader_h loadShader(const std::filesystem::path &shaderPath,
                                    const GLenum shaderType) {
    const shader_h hShader = glCreateShader(shaderType);
    if (hShader == 0) {
      throw std::runtime_error("Failed to create shader object");
    }

    const std::string shaderSrc =
        this->m_pVfs->readFile("shaders" / shaderPath);
    const char *shaderSrcCstr = shaderSrc.c_str();
    glShaderSource(hShader, 1, &shaderSrcCstr, nullptr);
    glCompileShader(hShader);
    int success;
    glGetShaderiv(hShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetShaderInfoLog(hShader, 512, nullptr, infoLog);
      throw std::runtime_error(fmt::format("Error compiling shader {0}: {1}",
                                           shaderPath.string(),
                                           std::string_view(infoLog)));
    }
    return hShader;
  }

public:
  Impl(const Window &window, const std::shared_ptr<VirtualFileSystem> _pVfs)
      : m_pVfs(_pVfs) {
    glbinding::initialize(window.glProcAddressFunc());
    glbinding::setAfterCallback(
        []([[maybe_unused]] const glbinding::FunctionCall &functionCall) {
          const auto errorCode = glbinding::Binding::GetError.directCall();

          if (errorCode == gl::GL_NO_ERROR) {
            return;
          }

          if (errorCode == GL_OUT_OF_MEMORY) {
            throw std::runtime_error("OpenGL driver reported out of memory");
          }

#ifndef NDEBUG
          const auto message =
              fmt::format("{0}: {1}", functionCall.function->name(),
                          readableErrorCode(errorCode));
          std::cerr << message << "\n";
          std::terminate();
#endif
        });
    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After,
                                     {"glGetError"});

    this->m_hShaderProgram = this->initShaderProgram();
    glUseProgram(this->m_hShaderProgram);
  }

  ~Impl() {
    glbinding::removeCallbackMaskExcept(glbinding::CallbackMask::After,
                                        {"glGetError"});
  }

  void setLayer(const LayerIdx layerN, const Layer &layer) {
    const auto mesh = layer.mesh();

    buffer_h vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    buffer_h vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    const auto nCoords = mesh.vertices.size() * 2;
    glBufferData(GL_ARRAY_BUFFER, nCoords * sizeof(float), &mesh.vertices[0],
                 GL_STATIC_DRAW);
    constexpr auto vertexPosAttrib = 0;
    glVertexAttribPointer(vertexPosAttrib, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(vertexPosAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    buffer_h ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    const auto nTriangleIndices = mesh.indices.size() * 3;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 nTriangleIndices * sizeof(unsigned int), &mesh.indices[0],
                 GL_STATIC_DRAW);

    // Finish working on the vertex array.
    glBindVertexArray(0);

    const auto layerIter = this->m_layers.begin();
    this->m_layers.insert(layerIter + layerN,
                          {
                              vao,
                              static_cast<unsigned int>(mesh.indices.size()),
                          });
  }

  void updateViewMatrix(const glm::mat4 &viewMatrix) const {
    const auto transformLoc =
        glGetUniformLocation(this->m_hShaderProgram, "viewMatrix");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  }

  void toggleWireframeMode() {
    m_wireframeModeEnabled = !m_wireframeModeEnabled;
    if (m_wireframeModeEnabled) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }

  void drawFrame() {
    this->clearScreen();

    for (const auto &layer : this->m_layers) {
      glBindVertexArray(layer.vao);
      glDrawElements(GL_TRIANGLES, static_cast<int>(layer.nIndices),
                     GL_UNSIGNED_INT, nullptr);
    }
    // Ensure that we keep the vertex array unbound just to keep global state
    // cleaned up.
    glBindVertexArray(0);
  }

  void resize(const int width, const int height) {
    glViewport(0, 0, width, height);
  }
}; // namespace mata

Renderer::Renderer(const Window &window,
                   const std::shared_ptr<VirtualFileSystem> _pVfs)
    : m_pImpl(std::make_unique<Impl>(window, _pVfs)) {}

Renderer::~Renderer() noexcept = default;

void Renderer::updateViewMatrix(const glm::mat4 &viewMatrix) {
  m_pImpl->updateViewMatrix(viewMatrix);
}

void Renderer::setLayer(const LayerIdx layerN, const Layer &layer) {
  m_pImpl->setLayer(layerN, layer);
}

void Renderer::toggleWireframeMode() { m_pImpl->toggleWireframeMode(); }

void Renderer::drawFrame() { m_pImpl->drawFrame(); }

void Renderer::resize(const int width, const int height) {
  m_pImpl->resize(width, height);
}

} // namespace mata
