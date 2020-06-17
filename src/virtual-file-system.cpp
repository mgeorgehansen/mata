#include <exception>
#include <string>
#include <filesystem>
#include <fstream>

#include "virtual-file-system.h"
#include "utils/string.h"

class VirtualFileSystem::Impl
{
    const std::filesystem::path rootPath;

public:
    Impl(const std::filesystem::path &rootPath) : rootPath(rootPath)
    {
        if (!rootPath.is_absolute())
        {
            throw new std::logic_error(format("root path is not absolute: %s", rootPath.string().c_str()));
        }
        if (!std::filesystem::exists(rootPath))
        {
            throw new std::runtime_error(format("root path does not exist: %s", rootPath.string().c_str()));
        }
        if (!std::filesystem::is_directory(rootPath))
        {
            throw new std::runtime_error(format("root path is not a directory: %s", rootPath.string().c_str()));
        }
    }

    std::string readFile(const std::filesystem::path &path)
    {
        if (!path.is_relative())
        {
            throw new std::logic_error(
                format("path must be relative: %s", path.string().c_str()));
        }
        const auto rootedPath = this->rootPath / path;
        if (!std::filesystem::exists(rootedPath))
        {
            throw new std::runtime_error(format("file not found in VFS: %s", path.string().c_str()));
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

VirtualFileSystem::~VirtualFileSystem(void) = default;

VirtualFileSystem::VirtualFileSystem(VirtualFileSystem &&) = default;

VirtualFileSystem &VirtualFileSystem::operator=(VirtualFileSystem &&) = default;

VirtualFileSystem::VirtualFileSystem(const std::filesystem::path &rootPath) : pImpl(std::make_unique<Impl>(rootPath)) {}

std::string VirtualFileSystem::readFile(const std::filesystem::path &path)
{
    return this->pImpl->readFile(path);
}
