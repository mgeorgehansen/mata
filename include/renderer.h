#pragma once

#include <experimental/propagate_const>
#include <memory>

class Renderer
{
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl;

public:
    Renderer();
    virtual ~Renderer();

    Renderer(Renderer &&);
    Renderer &operator=(Renderer &&);
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void run();
};
