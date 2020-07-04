/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <glbinding/glbinding.h>
#include <stdexcept>

#include <mata/app.hpp>
#include <mata/concepts/noncopyable.hpp>
#include <mata/renderer.hpp>
#include <mata/utils/filesystem.hpp>
#include <mata/utils/platform.hpp>

namespace mata {

[[noreturn]] void handleGlfwError(int, const char *);

[[noreturn]] void handleGlfwError(int errorCode, const char *message) {
  throw std::runtime_error(
      fmt::format("GLFW Error ({0}): {1}", errorCode, message));
}

class [[nodiscard]] App::Impl final : private noncopyable {
  GLFWwindow *m_pWindow{nullptr};
  std::unique_ptr<Renderer> m_pRenderer{nullptr};

  void processInput() const {
    if (glfwGetKey(this->m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(this->m_pWindow, true);
    }
  }

  void swapBuffers() const { glfwSwapBuffers(this->m_pWindow); }

public:
  Impl(const AppParams &params) {
    glfwSetErrorCallback(handleGlfwError);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if MATA_OS_MACOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (params.headless) {
      glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
      glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_OSMESA_CONTEXT_API);
      // NOTE: OSMesa does not support forward compat, so we disable it for
      //       macOS.
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    }

    try {
      this->m_pWindow = glfwCreateWindow(800, 600, "Mata", nullptr, nullptr);
    } catch (...) {
      std::throw_with_nested(
          std::runtime_error("Failed to create OpenGL context"));
    }
    glfwMakeContextCurrent(this->m_pWindow);

    glbinding::initialize(glfwGetProcAddress);

    const auto resourcesPath =
        params.resourcesPath.value_or(execDir() / "resources");
    const auto vfs = std::make_shared<VirtualFileSystem>(resourcesPath);
    this->m_pRenderer = std::make_unique<Renderer>(vfs);

    glfwSetWindowUserPointer(this->m_pWindow, this->m_pRenderer.get());
    glfwSetFramebufferSizeCallback(
        this->m_pWindow,
        [](GLFWwindow *pWindow, int width, int height) -> void {
          const auto renderer =
              reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(pWindow));
          renderer->resize(width, height);
        });
  }

  ~Impl() { glfwTerminate(); }

  void stepFrame() const {
    this->processInput();

    this->m_pRenderer->startFrame();

    this->m_pRenderer->pushGeometry({{
                                        0.5f, 0.5f, 0.0f,   // top right
                                        0.5f, -0.5f, 0.0f,  // bottom right
                                        -0.5f, -0.5f, 0.0f, // bottom left
                                        -0.5f, 0.5f, 0.0f   // top left
                                    }},
                                    {{
                                        0, 1, 3, // first triangle
                                        1, 2, 3  // second triangle
                                    }});

    this->m_pRenderer->endFrame();

    this->swapBuffers();

    glfwPollEvents();
  }

  void run() const {
    while (!glfwWindowShouldClose(this->m_pWindow)) {
      this->stepFrame();
    }
  }
};

App::App(const AppParams &params) : m_pImpl(std::make_unique<Impl>(params)) {}

App::~App() noexcept = default;

void App::stepFrame() const { m_pImpl->stepFrame(); }

void App::run() const { m_pImpl->run(); }

} // namespace mata
