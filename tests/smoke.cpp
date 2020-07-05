/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <stdexcept>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cstdlib>
#include <iostream>
#include <mata/lib.hpp>
#include <mata/utils/exceptions.hpp>

#include "config.hpp"

TEST_CASE("Smoke test", "[main]") {
  auto params = mata::AppParams{};
  params.headless = true;
  params.resourcesPath = MATA_RESOURCES_PATH;
  try {
    auto app = mata::App(params);
    app.stepFrame();
  } catch (const std::exception &error) {
    const auto message = mata::format_exception(error);
    throw std::runtime_error(message);
  }
}
