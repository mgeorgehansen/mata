/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include <mata/lib.hpp>

inline void print_exception(const std::exception &error, size_t level = 0) {
  if (0 == level) {
    std::cerr << "error: " << error.what() << '\n';
  } else {
    std::cerr << std::string(level, ' ') << "caused by: " << error.what()
              << '\n';
  }

  try {
    std::rethrow_if_nested(error);
  } catch (const std::exception &e) {
    print_exception(e, level + 1);
  } catch (...) {
  }
}

int main() {
  try {
    const mata::App app;
    app.run();
  } catch (const std::exception &error) {
    print_exception(error);
    return 1;
  }
  return 0;
}
