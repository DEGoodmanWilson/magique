//
// Created by Don Goodman-Wilson on 05/12/2017.
//

#include "../magique/catalog.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

magique::catalog catalog_limited{"tests/data/catalog_limited.json"};

TEST_CASE( "Are the tests building and running?") {
    REQUIRE( true );
}