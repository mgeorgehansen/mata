#pragma once

#include <experimental/propagate_const>
#include <memory>

class App
{
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl;

public:
    App();
    virtual ~App();

    App(App &&);
    App &operator=(App &&);
    App(const App &) = delete;
    App &operator=(const App &) = delete;

    void run() const;
};
