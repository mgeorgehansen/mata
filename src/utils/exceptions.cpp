/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <mata/utils/exceptions.hpp>

#include <exception>
#include <fmt/format.h>
#include <string>

namespace mata {

inline std::string _format_exception(const std::exception &error,
                                     const size_t level = 0) {
  auto output = std::string{};
  if (0 == level) {
    output.append(fmt::format("error: {}\n", error.what()));
  } else {
    output.append(fmt::format("{0}caused by: {1}\n", std::string(level, ' '),
                              error.what()));
  }

  try {
    std::rethrow_if_nested(error);
  } catch (const std::exception &e) {
    const auto nestedOutput = _format_exception(e, level + 1);
    output.append(nestedOutput);
  } catch (...) {
  }

  return output;
}

std::string format_exception(const std::exception &error) {
  return _format_exception(error, 0);
}

} // namespace mata
