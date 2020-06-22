#pragma once

#include <experimental/propagate_const>
#include <memory>

#include "concepts/noncopyable.h"

class App final: private noncopyable
{
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl;

public:
    App();
    ~App() noexcept;

    void run() const;
};
