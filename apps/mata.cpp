/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include <mata/lib.hpp>
#include <mata/utils/exceptions.hpp>

int main() {
  try {
    const mata::App app;
    app.run();
  } catch (const std::exception &error) {
    const auto errorMessage = mata::format_exception(error);
    std::cerr << errorMessage << "\n";
    return 1;
  }
  return 0;
}
