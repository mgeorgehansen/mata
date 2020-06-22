#pragma once

#include <string>
#include <experimental/propagate_const>
#include <memory>
#include <filesystem>

#include "concepts/noncopyable.h"

class VirtualFileSystem final: private noncopyable
{
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl;

public:
    VirtualFileSystem(const std::filesystem::path &rootPath);
    ~VirtualFileSystem() noexcept;

    std::string readFile(const std::filesystem::path &path) const;
};
