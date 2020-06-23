#include <exception>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <glbinding/gl33core/gl.h>
#include <glbinding/glbinding.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <string>

#include "concepts/noncopyable.h"
#include "renderer.h"
#include "virtual-file-system.h"

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
      throw new std::runtime_error(fmt::format(
          "Failed to link shader program: {0}", std::string_view(infoLog)));
    }
    glDeleteShader(hFragmentShader);
    glDeleteShader(hVertexShader);
    return hShaderProgram;
  }

  [[nodiscard]] shader_h loadShader(const std::filesystem::path &shaderPath,
                                    const GLenum shaderType) const {
    const shader_h hShader = glCreateShader(shaderType);
    if (hShader == 0) {
      throw new std::runtime_error("Failed to create shader object");
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
      throw new std::runtime_error(
          fmt::format("Error compiling shader {0}: {1}", shaderPath.string(),
                      std::string_view(infoLog)));
    }
    return hShader;
  }

  void drawTriangle() const {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f,
    };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glUseProgram(this->m_hShaderProgram);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

public:
  Impl(const std::shared_ptr<VirtualFileSystem> _pVfs) : m_pVfs(_pVfs) {
    this->m_hShaderProgram = this->initShaderProgram();
  }

  void drawFrame() const {
    this->clearScreen();

    this->drawTriangle();
  }

  void resize(const int width, const int height) const {
    glViewport(0, 0, width, height);
  }
};

Renderer::Renderer(const std::shared_ptr<VirtualFileSystem> _pVfs)
    : m_pImpl(std::make_unique<Impl>(_pVfs)) {}

Renderer::~Renderer() noexcept = default;

void Renderer::drawFrame() const { m_pImpl->drawFrame(); }

void Renderer::resize(const int width, const int height) const {
  m_pImpl->resize(width, height);
}
