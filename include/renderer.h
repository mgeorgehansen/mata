#pragma once

#include <experimental/propagate_const>
#include <memory>

#include "virtual-file-system.h"

class Renderer
{
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl;

public:
    Renderer(std::shared_ptr<VirtualFileSystem>);
    virtual ~Renderer();

    Renderer(Renderer &&);
    Renderer &operator=(Renderer &&);
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void run();
};
