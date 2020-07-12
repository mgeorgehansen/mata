/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <mata/layer.hpp>

TEST_CASE("returns right mesh", "[Layer]") {
  const auto layer = mata::Layer{2, 2};
  const auto expectedMesh = mata::Mesh2d{
      {0, 0, 1, 0, 2, 0, 0, 1, 1, 1, 2, 1, 0, 2, 1, 2, 2, 2},
      {0, 3, 1, 3, 4, 1, 1, 4, 2, 4, 5, 2, 3, 6, 4, 6, 7, 4, 4, 7, 5, 7, 8, 5}};
  REQUIRE(layer.mesh() == expectedMesh);
}
