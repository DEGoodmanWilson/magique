//
// Created by Don Goodman-Wilson on 05/12/2017.
//

#include <fstream>
#include <catch.hpp>
#include <nlohmann/json.hpp>
#include "../magique/card.h"

TEST_CASE("We can construct a card from some JSON")
{
    std::fstream ifs{"tests/data/card_holy_living_hell.json"};
    nlohmann::json json_obj{ifs};

    auto card = json_obj.get<magique::card>();
    REQUIRE(card.name == "Holy Living Hell");

    REQUIRE(card.types.count(magique::card::type::instant) == 1);
    REQUIRE(card.types.count(magique::card::type::creature) == 1);
    REQUIRE(card.types.count(magique::card::type::land) == 0);

    REQUIRE(card.subtypes.count("Human") == 1);
    REQUIRE(card.subtypes.count("Jester") == 1);
    REQUIRE(card.subtypes.count("Foobar") == 0);

    REQUIRE (card.has_type("Instant"));
    REQUIRE (card.has_type("Creature"));
    REQUIRE (!card.has_type("Land"));
}

TEST_CASE("Basic lands are lands too")
{
    std::fstream ifs{"tests/data/card_basic_land.json"};
    nlohmann::json json_obj{ifs};

    auto card = json_obj.get<magique::card>();
    REQUIRE(card.name == "Island");

    REQUIRE(card.types.count(magique::card::type::basic_land) == 1);
    REQUIRE(card.types.count(magique::card::type::land) == 1);

    REQUIRE(card.has_type("basic land"));
    REQUIRE(card.has_type("land"));
}

TEST_CASE("Non-basic lands are not basic lands, duh")
{
    std::fstream ifs{"tests/data/card_non_basic_land.json"};
    nlohmann::json json_obj{ifs};

    auto card = json_obj.get<magique::card>();
    REQUIRE(card.name == "City of Ass");

    REQUIRE(card.types.count(magique::card::type::basic_land) == 0);
    REQUIRE(card.types.count(magique::card::type::land) == 1);

    REQUIRE(!card.has_type("basic land"));
    REQUIRE(card.has_type("land"));
}

TEST_CASE("Cards pickle to JSON correctly")
{
    magique::card card;
    card.name = "Howdy";
    card.types = {magique::card::type::land};

    nlohmann::json card_obj = card;

    std::string card_json{card_obj.dump()};

    REQUIRE(card_json == R"({"cmc":0,"name":"Howdy","types":["Land"]})");
}