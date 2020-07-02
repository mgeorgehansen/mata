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
    // Ensure that we use OSMESA on Windows in headless mode for CI tests.
#if MATA_OS_WINDOWS
    if (params.headless) {
      glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_OSMESA_CONTEXT_API);
    }
#endif
#if MATA_OS_MACOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (params.headless) {
      glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }

    this->m_pWindow = glfwCreateWindow(800, 600, "Mata", nullptr, nullptr);
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

    this->m_pRenderer->drawFrame();

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
