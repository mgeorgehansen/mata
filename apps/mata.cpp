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
  auto params = mata::AppParams{};
  const auto resPath = std::getenv("RESOURECES_PATH");
  if (nullptr != resPath) {
    params.resourcesPath = std::string(resPath);
  }

  try {
    const auto app = mata::App(params);
    app.run();
  } catch (const std::exception &error) {
    const auto errorMessage = mata::format_exception(error);
    std::cerr << errorMessage << "\n";
    return 1;
  }
  return 0;
}
