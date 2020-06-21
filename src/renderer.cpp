#include <glbinding/glbinding.h>
#include <glbinding/gl33core/gl.h>
#include <string>
#include <iostream>
#include <exception>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <fmt/core.h>
#include <filesystem>

#include "renderer.h"
#include "virtual-file-system.h"

using namespace gl;

typedef GLuint shaderprogram_h;
typedef GLuint shader_h;

class Renderer::Impl
{
    const std::shared_ptr<VirtualFileSystem> pVfs;
    shaderprogram_h hShaderProgram = 0;

    void clearScreen()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(ClearBufferMask::GL_COLOR_BUFFER_BIT);
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
            throw new std::runtime_error(fmt::format(
                "Failed to link shader program: {0}", std::string_view(infoLog)));
        }
        glDeleteShader(hFragmentShader);
        glDeleteShader(hVertexShader);
        return hShaderProgram;
    }

    shader_h loadShader(const std::filesystem::path& shaderPath, GLenum shaderType)
    {
        const shader_h hShader = glCreateShader(shaderType);
        if (hShader == 0)
        {
            throw new std::runtime_error("Failed to create shader object");
        }

        const std::string shaderSrc = this->pVfs->readFile(
            "shaders" / shaderPath);
        const char *shaderSrcCstr = shaderSrc.c_str();
        glShaderSource(hShader, 1, &shaderSrcCstr, NULL);
        glCompileShader(hShader);
        int success;
        char infoLog[512];
        glGetShaderiv(hShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(hShader, 512, NULL, infoLog);
            throw new std::runtime_error(fmt::format(
                "Error compiling shader {0}: {1}", shaderPath.string(), std::string_view(infoLog)));
        }
        return hShader;
    }

    void drawTriangle()
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
    Impl(std::shared_ptr<VirtualFileSystem> _pVfs) : pVfs(_pVfs)
    {
        this->hShaderProgram = this->initShaderProgram();
    }
    ~Impl() = default;

    void drawFrame()
    {
        this->clearScreen();

        this->drawTriangle();
    }

    void resize(const int width, const int height)
    {
        glViewport(0, 0, width, height);
    }
};

Renderer::~Renderer() = default;

Renderer::Renderer(Renderer &&) = default;

Renderer &Renderer::operator=(Renderer &&) = default;

Renderer::Renderer(std::shared_ptr<VirtualFileSystem> _pVfs) : pImpl(std::make_unique<Impl>(_pVfs)) {}

void Renderer::drawFrame()
{
    pImpl->drawFrame();
}

void Renderer::resize(const int width, const int height)
{
    pImpl->resize(width, height);
}
