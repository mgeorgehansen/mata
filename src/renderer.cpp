#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <exception>
#include <memory>

#include "renderer.h"

void framebuffer_size_callback(GLFWwindow *pWindow, int width, int height)
{
    glViewport(0, 0, width, height);
}

class Renderer::Impl
{
    GLFWwindow *pWindow;

    void processInput()
    {
        if (glfwGetKey(this->pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(this->pWindow, true);
        }
    }

    void swapBuffers()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(this->pWindow);
    }

public:
    Impl()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // for mac.
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        this->pWindow = glfwCreateWindow(800, 600, "Mata", NULL, NULL);
        if (this->pWindow == NULL)
        {
            throw new std::runtime_error("Failed to create GLFW Window");
        }
        glfwMakeContextCurrent(this->pWindow);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw new std::runtime_error("Failed to initialize GLAD");
        }

        glfwSetFramebufferSizeCallback(this->pWindow, framebuffer_size_callback);
    }
    ~Impl()
    {
        glfwTerminate();
    }

    void run()
    {
        while (!glfwWindowShouldClose(this->pWindow))
        {
            this->processInput();

            this->swapBuffers();

            glfwPollEvents();
        }
    }
};

Renderer::~Renderer(void) = default;

Renderer::Renderer(Renderer &&) = default;

Renderer &Renderer::operator=(Renderer &&) = default;

Renderer::Renderer(void) : pImpl(std::make_unique<Impl>()) {}

void Renderer::run(void)
{
    pImpl->run();
}
