/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <GLFW/glfw3.h>
#include <glbinding/glbinding.h>

#include <mata/app.hpp>
#include <mata/concepts/noncopyable.hpp>
#include <mata/renderer.hpp>
#include <mata/utils/filesystem.hpp>

namespace mata {

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
  Impl() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // for mac.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    this->m_pWindow = glfwCreateWindow(800, 600, "Mata", nullptr, nullptr);
    if (nullptr == this->m_pWindow) {
      throw new std::runtime_error("Failed to create GLFW Window");
    }
    glfwMakeContextCurrent(this->m_pWindow);

    glbinding::initialize(glfwGetProcAddress);

    const auto vfs =
        std::make_shared<VirtualFileSystem>(execDir() / "resources");
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

  void run() const {
    while (!glfwWindowShouldClose(this->m_pWindow)) {
      this->processInput();

      this->m_pRenderer->drawFrame();

      this->swapBuffers();

      glfwPollEvents();
    }
  }
};

App::App() : m_pImpl(std::make_unique<Impl>()) {}

App::~App() noexcept = default;

void App::run() const { m_pImpl->run(); }

} // namespace mata
