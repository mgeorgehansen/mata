#pragma once

#include <experimental/propagate_const>
#include <filesystem>
#include <memory>
#include <string>

#include "concepts/noncopyable.h"

class [[nodiscard]] VirtualFileSystem final : private noncopyable {
  class Impl;
  std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl;

public:
  VirtualFileSystem(const std::filesystem::path &rootPath);
  ~VirtualFileSystem() noexcept;

  [[nodiscard]] std::string readFile(const std::filesystem::path &path) const;
};
