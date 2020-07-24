/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <exception>
#include <filesystem>
#include <vector>

#include "mata/platform/filesystem.hpp"
#include "mata/platform/platform.hpp"

#if MATA_OS_MACOS
#include <mach-o/dyld.h>
#elif MATA_OS_WINDOWS
#include <Windows.h>
#elif MATA_OS_LINUX
#include <limits.h>
#include <unistd.h>
#else
#error "Unknown platform is unsupported"
#endif

namespace mata {
namespace platform {

[[nodiscard]] static std::filesystem::path execPath() {
#if MATA_OS_MACOS
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);
  std::vector<char> buffer(size);
  _NSGetExecutablePath(&buffer[0], &size);
  const std::filesystem::path path(buffer.begin(), buffer.end());
  return std::filesystem::canonical(path);
#elif MATA_OS_WINDOWS
  std::vector<char> buffer(1024);
  auto size = buffer.size();
  auto havePath = false;
  auto shouldContinue = true;
  // Keep trying to load the exec path into buffer, doubling the buffer size
  // each time if the buffer is too small.
  // TODO: limit number of resizes to prevent unbounded memory usage.
  do {
    auto result = GetModuleFileNameA(nullptr, &buffer[0], size);
    auto lastError = GetLastError();
    if (result == 0) {
      shouldContinue = false;
    } else if (result < size) {
      havePath = true;
      shouldContinue = false;
    } else if (result == size && (lastError == ERROR_INSUFFICIENT_BUFFER ||
                                  lastError == ERROR_SUCCESS)) {
      // Path is larger than our buffer, so double the buffer and try again.
      size *= 2;
      buffer.resize(size);
    } else {
      shouldContinue = false;
    }
  } while (shouldContinue);
  if (!havePath) {
    throw std::runtime_error("failed to find exec path");
  }
  return std::filesystem::path(buffer.begin(), buffer.end());
#elif MATA_OS_LINUX
  char result[PATH_MAX];
  const ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::string(result, static_cast<unsigned long>(count > 0 ? count : 0));
#endif
}

[[nodiscard]] std::filesystem::path execDir() {
  auto path = execPath();
  return path.remove_filename();
}

} // namespace platform
} // namespace mata
