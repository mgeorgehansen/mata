#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <exception>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include "renderer.h"
#include "virtual-file-system.h"
#include "utils/string.h"

typedef GLuint shaderprogram_h;
typedef GLuint shader_h;

void framebuffer_size_callback(GLFWwindow *pWindow, int width, int height)
{
    glViewport(0, 0, width, height);
}

class Renderer::Impl
{
    GLFWwindow *pWindow;
    const std::shared_ptr<VirtualFileSystem> pVfs;
    shaderprogram_h hShaderProgram;

    void processInput()
    {
        if (glfwGetKey(this->pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(this->pWindow, true);
        }
    }

    void clearScreen()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void swapBuffers()
    {
        glfwSwapBuffers(this->pWindow);
    }

    shaderprogram_h initShaderProgram()
    {
        const auto hVertexShader = this->loadShader("default.vert", GL_VERTEX_SHADER);
        const auto hFragmentShader = this->loadShader("default.frag", GL_FRAGMENT_SHADER);
        const auto hShaderProgram = glCreateProgram();
        glAttachShader(hShaderProgram, hVertexShader);
        glAttachShader(hShaderProgram, hFragmentShader);
        glLinkProgram(hShaderProgram);
        int success;
        char infoLog[512];
        glGetProgramiv(hShaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(hShaderProgram, 512, NULL, infoLog);
            throw new std::runtime_error(format(
                "Failed to link shader program: %s", &infoLog));
        }
        glDeleteShader(hFragmentShader);
        glDeleteShader(hVertexShader);
        return hShaderProgram;
    }

    shader_h loadShader(std::string shaderName, GLenum shaderType)
    {
        const shader_h hShader = glCreateShader(shaderType);
        if (hShader == 0)
        {
            throw new std::runtime_error("Failed to create shader object");
        }

        const std::string shaderSrc = this->pVfs->readFile(
            format("shaders/%s", shaderName.c_str()));
        const char *shaderSrcCstr = shaderSrc.c_str();
        glShaderSource(hShader, 1, &shaderSrcCstr, NULL);
        glCompileShader(hShader);
        int success;
        char infoLog[512];
        glGetShaderiv(hShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(hShader, 512, NULL, infoLog);
            throw new std::runtime_error(format(
                "Error compiling shader %s: %s", shaderName.c_str(), &infoLog));
        }
        return hShader;
    }

    void draw()
    {
        float vertices[] = {
            -0.5f,
            -0.5f,
            0.0f,
            0.5f,
            -0.5f,
            0.0f,
            0.0f,
            0.5f,
            0.0f,
        };

        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glUseProgram(this->hShaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

public:
    Impl(std::shared_ptr<VirtualFileSystem> _pVfs) : pWindow(NULL), pVfs(_pVfs), hShaderProgram(0)
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

        this->hShaderProgram = this->initShaderProgram();
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

            this->clearScreen();

            this->draw();

            this->swapBuffers();

            glfwPollEvents();
        }
    }
};

Renderer::~Renderer(void) = default;

Renderer::Renderer(Renderer &&) = default;

Renderer &Renderer::operator=(Renderer &&) = default;

Renderer::Renderer(std::shared_ptr<VirtualFileSystem> _pVfs) : pImpl(std::make_unique<Impl>(_pVfs)) {}

void Renderer::run(void)
{
    pImpl->run();
}
