#pragma once

#include <exception>
#include <filesystem>
#include <mach-o/dyld.h>

[[nodiscard]] std::filesystem::path execPath() {
  // TODO: implement cross-platform.
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);
  const auto buffer = new char[size];
  _NSGetExecutablePath(buffer, &size);
  return std::filesystem::canonical(buffer);
}

[[nodiscard]] std::filesystem::path execDir() {
  auto path = execPath();
  return path.remove_filename();
}
