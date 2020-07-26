/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <exception>
#include <string>

#include <fmt/format.h>

#include "mata/exceptions.hpp"

namespace mata {

std::string format_exception(const std::exception &error,
                             const std::size_t level) {
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
    const auto nestedOutput = format_exception(e, level + 1);
    output.append(nestedOutput);
  } catch (...) {
  }

  return output;
}

} // namespace mata
