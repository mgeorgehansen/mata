/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <exception>
#include <filesystem>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(_MSC_VER)
#include <Windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#include <mata/utils/filesystem.hpp>

namespace mata {

[[nodiscard]] static std::filesystem::path execPath() {
#if defined(__APPLE__)
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);
  const auto buffer = new char[size];
  _NSGetExecutablePath(buffer, &size);
  return std::filesystem::canonical(buffer);
#elif defined(_MSVC_VER)
  std::vector<char> buf(1024, 0);
  auto size = buf.size();
  auto havePath = false;
  auto shouldContinue = true;
  // Keep trying to load the exec path into buffer, doubling the buffer size
  // each time if the buffer is too small.
  // TODO: limit number of resizes to prevent unbounded memory usage.
  do {
    auto result = GetModuleFileNameA(nullptr, &buf[0], size);
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
      buf.resize(size);
    } else {
      shouldContinue = false;
    }
  } while (shouldContinue);
#else
  char result[PATH_MAX];
  const ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::string(result, static_cast<unsigned long>(count > 0 ? count : 0));
#endif
}

[[nodiscard]] std::filesystem::path execDir() {
  auto path = execPath();
  return path.remove_filename();
}

} // namespace mata
