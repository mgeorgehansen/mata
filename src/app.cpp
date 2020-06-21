#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>

#include "app.h"
#include "renderer.h"
#include "utils/filesystem.h"

class App::Impl
{
    GLFWwindow *pWindow;
    std::shared_ptr<Renderer> pRenderer;

    void processInput() const
    {
        if (glfwGetKey(this->pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(this->pWindow, true);
        }
    }

    void swapBuffers() const
    {
        glfwSwapBuffers(this->pWindow);
    }

public:
    Impl(): pWindow(nullptr), pRenderer()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // for mac.
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        this->pWindow = glfwCreateWindow(800, 600, "Mata", nullptr, nullptr);
        if (nullptr == this->pWindow)
        {
            throw new std::runtime_error("Failed to create GLFW Window");
        }
        glfwMakeContextCurrent(this->pWindow);

        glbinding::initialize(glfwGetProcAddress);

        const auto vfs = std::make_shared<VirtualFileSystem>(
            execDir() / "resources");
        const auto pRenderer = this->pRenderer = std::make_shared<Renderer>(vfs);

        glfwSetWindowUserPointer(this->pWindow, pRenderer.get());
        glfwSetFramebufferSizeCallback(this->pWindow, [](GLFWwindow *pWindow, int width, int height) -> void {
            const auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(pWindow));
            renderer->resize(width, height);
        });
    }

    ~Impl()
    {
        glfwTerminate();
    }

    void run() const
    {
        while (!glfwWindowShouldClose(this->pWindow))
        {
            this->processInput();

            this->pRenderer->drawFrame();

            this->swapBuffers();

            glfwPollEvents();
        }
    }
};

App::~App() = default;

App::App(App &&) = default;

App &App::operator=(App &&) = default;

App::App() : pImpl(std::make_unique<Impl>()) {}


void App::run() const
{
    pImpl->run();
}
