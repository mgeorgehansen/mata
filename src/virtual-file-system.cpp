#include <exception>
#include <string>
#include <filesystem>
#include <fstream>
#include <fmt/core.h>

#include "virtual-file-system.h"
#include "concepts/noncopyable.h"

class [[nodiscard]] VirtualFileSystem::Impl final: noncopyable
{
    std::filesystem::path rootPath;

public:
    Impl(const std::filesystem::path &rootPath) : rootPath(rootPath)
    {
        if (!rootPath.is_absolute())
        {
            throw new std::logic_error(fmt::format("root path is not absolute: {0}", rootPath.string()));
        }
        if (!std::filesystem::exists(rootPath))
        {
            throw new std::runtime_error(fmt::format("root path does not exist: {0}", rootPath.string()));
        }
        if (!std::filesystem::is_directory(rootPath))
        {
            throw new std::runtime_error(fmt::format("root path is not a directory: {0}", rootPath.string()));
        }
    }

    [[nodiscard]] std::string readFile(const std::filesystem::path &path) const
    {
        if (!path.is_relative())
        {
            throw new std::logic_error(
                fmt::format("path must be relative: {0}", path.string()));
        }
        const auto rootedPath = this->rootPath / path;
        if (!std::filesystem::exists(rootedPath))
        {
            throw new std::runtime_error(fmt::format("file not found in VFS: {0}", path.string()));
        }
        std::ifstream inputFile(rootedPath);
        inputFile.exceptions(std::ios_base::badbit);
        std::string fileContents;
        constexpr std::size_t readSize(4096);
        std::string buffer(readSize, '\0');
        while (inputFile.read(&buffer[0], readSize))
        {
            fileContents.append(buffer, 0, inputFile.gcount());
        }
        fileContents.append(buffer, 0, inputFile.gcount());
        return fileContents;
    }
};

VirtualFileSystem::VirtualFileSystem(const std::filesystem::path &rootPath) : pImpl(std::make_unique<Impl>(rootPath)) {}

VirtualFileSystem::~VirtualFileSystem() noexcept = default;

[[nodiscard]] std::string VirtualFileSystem::readFile(const std::filesystem::path &path) const
{
    return this->pImpl->readFile(path);
}
