/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <functional>
#include <memory>

#include <mata/concepts/noncopyable.hpp>
#include <mata/utils/platform.hpp>
#include <mata/window.hpp>

namespace mata {

[[noreturn]] inline void handleGlfwError(int errorCode, const char *message) {
  throw std::runtime_error(
      fmt::format("GLFW Error ({0}): {1}", errorCode, message));
}

class [[nodiscard]] Window::Impl final : private noncopyable {
private:
  GLFWwindow *m_pWindow{nullptr};
  ResizeCallback m_resizeCallback{nullptr};
  WindowCloseRequestedCallback m_windowCloseRequestedCallback{nullptr};
  KeyEventCallback m_keyEventCallback{nullptr};
  double m_lastFrameTimestamp = -1.0;

public:
  Impl(const bool headless) {
    glfwSetErrorCallback(handleGlfwError);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if MATA_OS_MACOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (headless) {
      glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
      glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_OSMESA_CONTEXT_API);
      // NOTE: OSMesa does not support forward compat, so we disable it for
      //       macOS.
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    }

    try {
      m_pWindow = glfwCreateWindow(800, 600, "Mata", nullptr, nullptr);
    } catch (...) {
      std::throw_with_nested(
          std::runtime_error("Failed to create OpenGL context"));
    }
    glfwMakeContextCurrent(m_pWindow);
    glfwSetWindowUserPointer(m_pWindow, this);
  }

  ~Impl() {
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
  }

  void update() {
    glfwSwapBuffers(m_pWindow);
    glfwPollEvents();
    m_lastFrameTimestamp = glfwGetTime();
  }

  void onResize(const ResizeCallback callback) {
    m_resizeCallback = callback;
    glfwSetFramebufferSizeCallback(m_pWindow, [](GLFWwindow *pWindow, int width,
                                                 int height) {
      auto instance = static_cast<Impl *>(glfwGetWindowUserPointer(pWindow));
      // FIXME: integer overflow/underflow
      std::invoke(instance->m_resizeCallback, static_cast<unsigned int>(width),
                  static_cast<unsigned int>(height));
    });
  }

  void onWindowCloseRequested(const WindowCloseRequestedCallback callback) {
    m_windowCloseRequestedCallback = callback;
    glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow *pWindow) {
      auto instance = static_cast<Impl *>(glfwGetWindowUserPointer(pWindow));
      std::invoke(instance->m_windowCloseRequestedCallback);
    });
  }

  void onKeyEvent(const KeyEventCallback callback) {
    m_keyEventCallback = callback;
    glfwSetKeyCallback(m_pWindow, [](GLFWwindow *pWindow, int key,
                                     [[maybe_unused]] int scancode, int action,
                                     [[maybe_unused]] int mods) {
      auto instance = static_cast<Impl *>(glfwGetWindowUserPointer(pWindow));
      std::invoke(instance->m_keyEventCallback, key, action);
    });
  }
};

Window::Window(const bool headless)
    : m_pImpl(std::make_unique<Impl>(headless)) {}

Window::~Window() noexcept = default;

GlProcAddressFunc Window::glProcAddressFunc() const {
  return glfwGetProcAddress;
}

void Window::onResize(const ResizeCallback callback) {
  m_pImpl->onResize(callback);
}

void Window::onWindowCloseRequested(
    const WindowCloseRequestedCallback callback) {
  m_pImpl->onWindowCloseRequested(callback);
}

void Window::onKeyEvent(const KeyEventCallback callback) {
  m_pImpl->onKeyEvent(callback);
}

void Window::update() { m_pImpl->update(); }

} // namespace mata
