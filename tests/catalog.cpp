//
// Created by Don Goodman-Wilson on 07/12/2017.
//

#include "../magique/catalog.h"
#include <catch.hpp>

TEST_CASE("We can construct a catalog from a simple JSON file, without annotations")
{
    magique::catalog catalog{"tests/data/catalog_simple.json"};

    auto card1 = catalog.at("Atinlay Igpay"); //will throw an out of bounds exception of some kind of this fails

    REQUIRE(card1.name == "Atinlay Igpay");
}

TEST_CASE("We can construct a catalog from a simple JSON file, WITH annotations")
{
    magique::catalog catalog{"tests/data/catalog_simple.json", "tests/data/annotations_simple.json"};

    auto card1 = catalog.at("Atinlay Igpay"); //will throw an out of bounds exception of some kind of this fails

    REQUIRE(card1.abilities.count("one") == 1);
    REQUIRE(card1.abilities.count("two") == 1);
    REQUIRE(card1.abilities.count("three") == 0);

    REQUIRE(card1.affinities.count("five") == 1);
    REQUIRE(card1.affinities.count("six") == 1);
    REQUIRE(card1.affinities.count("seven") == 0);
}