/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <exception>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <iterator>
#include <string>

#include <mata/concepts/noncopyable.hpp>
#include <mata/types.hpp>
#include <mata/virtual-file-system.hpp>

namespace mata {

class [[nodiscard]] VirtualFileSystem::Impl final : noncopyable {
  std::filesystem::path m_rootPath;

public:
  Impl(const std::filesystem::path &rootPath) : m_rootPath(rootPath) {
    if (!rootPath.is_absolute()) {
      throw std::logic_error(
          fmt::format("root path is not absolute: {0}", rootPath.string()));
    }
    if (!std::filesystem::exists(rootPath)) {
      throw std::runtime_error(
          fmt::format("root path does not exist: {0}", rootPath.string()));
    }
    if (!std::filesystem::is_directory(rootPath)) {
      throw std::runtime_error(
          fmt::format("root path is not a directory: {0}", rootPath.string()));
    }
  }

  [[nodiscard]] bytes readFile(const std::filesystem::path &path) const {
    if (!path.is_relative()) {
      throw std::logic_error(
          fmt::format("path must be relative: {0}", path.string()));
    }
    const auto rootedPath = this->m_rootPath / path;
    if (!std::filesystem::exists(rootedPath)) {
      throw std::runtime_error(
          fmt::format("file not found in VFS: {0}", path.string()));
    }

    auto inputFile = std::ifstream(rootedPath, std::ios_base::binary);
    inputFile.exceptions(std::ios_base::badbit);

    // NOTE: we implicly convert between char and unsigned char. This *should*
    // be safe because negative bytes from ifstream seems unlikely or
    // impossible.
    auto bufferIter = std::istreambuf_iterator<char>(inputFile);
    const auto bufferIterEnd = std::istreambuf_iterator<char>();
    return bytes(bufferIter, bufferIterEnd);
  }

  [[nodiscard]] std::string readTextFile(const std::filesystem::path &path)
      const {
    const auto bytes = this->readFile(path);
    return std::string(bytes.begin(), bytes.end());
  }
};

VirtualFileSystem::VirtualFileSystem(const std::filesystem::path &rootPath)
    : m_pImpl(std::make_unique<Impl>(rootPath)) {}

VirtualFileSystem::~VirtualFileSystem() noexcept = default;

bytes VirtualFileSystem::readFile(const std::filesystem::path &path) const {
  return m_pImpl->readFile(path);
}

std::string
VirtualFileSystem::readTextFile(const std::filesystem::path &path) const {
  return m_pImpl->readTextFile(path);
}

} // namespace mata
