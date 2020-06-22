#include <GLFW/glfw3.h>
#include <glbinding/glbinding.h>

#include "app.h"
#include "concepts/noncopyable.h"
#include "renderer.h"
#include "utils/filesystem.h"

class [[nodiscard]] App::Impl final : private noncopyable {
  GLFWwindow *pWindow;
  std::unique_ptr<Renderer> pRenderer;

  void processInput() const {
    if (glfwGetKey(this->pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(this->pWindow, true);
    }
  }

  void swapBuffers() const { glfwSwapBuffers(this->pWindow); }

public:
  Impl() : pWindow(nullptr), pRenderer() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // for mac.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    this->pWindow = glfwCreateWindow(800, 600, "Mata", nullptr, nullptr);
    if (nullptr == this->pWindow) {
      throw new std::runtime_error("Failed to create GLFW Window");
    }
    glfwMakeContextCurrent(this->pWindow);

    glbinding::initialize(glfwGetProcAddress);

    const auto vfs =
        std::make_shared<VirtualFileSystem>(execDir() / "resources");
    this->pRenderer = std::make_unique<Renderer>(vfs);

    glfwSetWindowUserPointer(this->pWindow, this->pRenderer.get());
    glfwSetFramebufferSizeCallback(
        this->pWindow, [](GLFWwindow *pWindow, int width, int height) -> void {
          const auto renderer =
              reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(pWindow));
          renderer->resize(width, height);
        });
  }

  ~Impl() { glfwTerminate(); }

  void run() const {
    while (!glfwWindowShouldClose(this->pWindow)) {
      this->processInput();

      this->pRenderer->drawFrame();

      this->swapBuffers();

      glfwPollEvents();
    }
  }
};

App::App() : pImpl(std::make_unique<Impl>()) {}

App::~App() noexcept = default;

void App::run() const { pImpl->run(); }
