/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <exception>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <glbinding/gl33core/gl.h>
#include <glbinding/glbinding.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

#include <mata/concepts/noncopyable.hpp>
#include <mata/renderer.hpp>
#include <mata/virtual-file-system.hpp>

namespace mata {

using namespace gl;

typedef GLuint shaderprogram_h;
typedef GLuint shader_h;

class [[nodiscard]] Renderer::Impl final : private noncopyable {
  std::shared_ptr<VirtualFileSystem> m_pVfs;
  shaderprogram_h m_hShaderProgram{0};

  void clearScreen() const {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(ClearBufferMask::GL_COLOR_BUFFER_BIT);
  }

  [[nodiscard]] shaderprogram_h initShaderProgram() const {
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
                                    const GLenum shaderType) const {
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
  Impl(const std::shared_ptr<VirtualFileSystem> _pVfs) : m_pVfs(_pVfs) {
    this->m_hShaderProgram = this->initShaderProgram();
  }

  void updateViewMatrix(const glm::mat4 &transform) const {
    const auto transformLoc =
        glGetUniformLocation(this->m_hShaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
  }

  void startFrame() const { this->clearScreen(); }

  void pushGeometry(const std::vector<float> &vertices,
                    const std::vector<unsigned int> &indices) const {
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0],
                 GL_STATIC_DRAW);
    // Vertex pos attrib.
    constexpr auto vertexPosAttrib = 0;
    glVertexAttribPointer(vertexPosAttrib, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(vertexPosAttrib);
    // Vertex color attrib.
    constexpr auto vertexColorAttrib = 1;
    constexpr auto vertexColorOffset = 3 * sizeof(float);
    glVertexAttribPointer(vertexColorAttrib, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float),
                          reinterpret_cast<void *>(vertexColorOffset));
    glEnableVertexAttribArray(vertexColorAttrib);

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    glUseProgram(this->m_hShaderProgram);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()),
                   GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }

  void endFrame() const {}

  void resize(const int width, const int height) const {
    glViewport(0, 0, width, height);
  }
};

Renderer::Renderer(const std::shared_ptr<VirtualFileSystem> _pVfs)
    : m_pImpl(std::make_unique<Impl>(_pVfs)) {}

Renderer::~Renderer() noexcept = default;

void Renderer::updateViewMatrix(const glm::mat4 &transform) const {
  m_pImpl->updateViewMatrix(transform);
}

void Renderer::startFrame() const { m_pImpl->startFrame(); }

void Renderer::pushGeometry(const std::vector<float> &vertices,
                            const std::vector<unsigned int> &indices) const {
  m_pImpl->pushGeometry(vertices, indices);
}

void Renderer::endFrame() const { m_pImpl->endFrame(); }

void Renderer::resize(const int width, const int height) const {
  m_pImpl->resize(width, height);
}

} // namespace mata
