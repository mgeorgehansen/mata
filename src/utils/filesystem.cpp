#include <exception>
#include <filesystem>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#include <mata/utils/filesystem.hpp>

namespace mata {

[[nodiscard]] static std::filesystem::path execPath() {
#if __APPLE__
  // TODO: implement cross-platform.
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);
  const auto buffer = new char[size];
  _NSGetExecutablePath(buffer, &size);
  return std::filesystem::canonical(buffer);
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
