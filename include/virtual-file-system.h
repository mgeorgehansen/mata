#pragma once

#include <string>
#include <experimental/propagate_const>
#include <memory>
#include <filesystem>

class VirtualFileSystem final
{
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl;

public:
    VirtualFileSystem(const std::filesystem::path &rootPath);
    ~VirtualFileSystem();

    VirtualFileSystem(VirtualFileSystem &&);
    VirtualFileSystem &operator=(VirtualFileSystem &&);
    VirtualFileSystem(const VirtualFileSystem &) = delete;
    VirtualFileSystem &operator=(const VirtualFileSystem &) = delete;

    std::string readFile(const std::filesystem::path &path) const;
};
