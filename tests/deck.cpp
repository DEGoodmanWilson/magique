//
// Created by Don Goodman-Wilson on 07/12/2017.
//

#include "../magique/deck.h"
#include <catch.hpp>

extern magique::catalog catalog_limited;

TEST_CASE("We can construct a deck from a list of indices into a collection")
{
    magique::collection collection{"tests/data/collection_limited.csv", catalog_limited};
    std::vector<uint64_t> indices{0};
    magique::interactions interactions;

    magique::deck deck{indices, collection, interactions};

    REQUIRE(deck.evaluate() == 12.0); //how is this 12?
}