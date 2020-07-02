/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <stdexcept>
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <cstdlib>
#include <iostream>
#include <mata/lib.hpp>
#include <mata/utils/exceptions.hpp>

static auto resourcesPath = std::string{};
static auto headless = false;

int main(int argc, char *argv[]) {
  auto session = Catch::Session{};

  using namespace Catch::clara;
  auto cli = session.cli() |
             Opt(resourcesPath, "resourcesPath")["--resourcesPath"](
                 "path to the resources folder") |
             Opt(headless)["--headless"](
                 "whether to run in headless mode (e.g. in CI)");
  session.cli(cli);

  // Parse command line args.
  const auto returnCode = session.applyCommandLine(argc, argv);
  if (0 != returnCode) {

    return returnCode;
  }
  if (resourcesPath.compare("") == 0) {
    std::cerr << "must specify --resourecesPath param";
    return EXIT_FAILURE;
  }

  return session.run();
}

TEST_CASE("Smoke test", "[main]") {
  auto params = mata::AppParams{};
  params.headless = headless;
  params.resourcesPath = resourcesPath;
  try {
    const auto app = mata::App(params);
    app.stepFrame();
  } catch (const std::exception &error) {
    const auto message = mata::format_exception(error);
    throw std::runtime_error(message);
  }
}
