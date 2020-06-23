#include <exception>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <string>

#include <mata/concepts/noncopyable.hpp>
#include <mata/virtual-file-system.hpp>

namespace mata {

class [[nodiscard]] VirtualFileSystem::Impl final : noncopyable {
  std::filesystem::path m_rootPath;

public:
  Impl(const std::filesystem::path &rootPath) : m_rootPath(rootPath) {
    if (!rootPath.is_absolute()) {
      throw new std::logic_error(
          fmt::format("root path is not absolute: {0}", rootPath.string()));
    }
    if (!std::filesystem::exists(rootPath)) {
      throw new std::runtime_error(
          fmt::format("root path does not exist: {0}", rootPath.string()));
    }
    if (!std::filesystem::is_directory(rootPath)) {
      throw new std::runtime_error(
          fmt::format("root path is not a directory: {0}", rootPath.string()));
    }
  }

  [[nodiscard]] std::string readFile(const std::filesystem::path &path) const {
    if (!path.is_relative()) {
      throw new std::logic_error(
          fmt::format("path must be relative: {0}", path.string()));
    }
    const auto rootedPath = this->m_rootPath / path;
    if (!std::filesystem::exists(rootedPath)) {
      throw new std::runtime_error(
          fmt::format("file not found in VFS: {0}", path.string()));
    }
    std::ifstream inputFile(rootedPath);
    inputFile.exceptions(std::ios_base::badbit);
    std::string fileContents;
    constexpr std::size_t readSize(4096);
    std::string buffer(readSize, '\0');
    while (inputFile.read(&buffer[0], readSize)) {
      // KLUDGE: gcount() should never return a negative integer, so we're safe
      // to static_cast here. Right...?
      fileContents.append(buffer, 0, static_cast<size_t>(inputFile.gcount()));
    }
    fileContents.append(buffer, 0, static_cast<size_t>(inputFile.gcount()));
    return fileContents;
  }
};

VirtualFileSystem::VirtualFileSystem(const std::filesystem::path &rootPath)
    : m_pImpl(std::make_unique<Impl>(rootPath)) {}

VirtualFileSystem::~VirtualFileSystem() noexcept = default;

[[nodiscard]] std::string
VirtualFileSystem::readFile(const std::filesystem::path &path) const {
  return this->m_pImpl->readFile(path);
}

} // namespace mata
