#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <mata/lib.hpp>

TEST_CASE( "Quick check", "[main]" ) {
    REQUIRE( 2.0 == 2.0 );
}
