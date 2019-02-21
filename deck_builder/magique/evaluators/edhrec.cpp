//
// Created by Donald Goodman-Wilson on 2019-02-17.
//

#include "edhrec.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>


namespace magique
{
namespace evaluators
{

struct edhrec_item
{
    uint deck_count;
    uint rank;
    double price;
    std::map<std::string, double> synergies;
};

static std::map<std::string, edhrec_item> edhrec_data;
static uint64_t lowest_rank{0};
static double_t highest_price{0.0};
static uint64_t most_decks{0};


void load_edhrec(std::string path)
{
//    // TODO YAML takes too long to load, do this faster by converting to JSON first
//    std::cerr << "Loading EDHREC data...";
//    auto edhrec_yaml = YAML::LoadFile(path + "/EDHREC.yaml");
//    edhrec_data = edhrec_yaml.as<decltype(edhrec_data)>();
//
//    // calculate individual values. Find:
//    // most expensive card
//    // lowest ranked card
//    // most number of decks
//    for(const auto card : edhrec_data)
//    {
//        if(card.second.edhrec_rank > lowest_rank)
//            lowest_rank = card.second.edhrec_rank;
//        if(card.second.price > highest_price)
//            highest_price = card.second.price;
//        if(card.second.deck_count > most_decks)
//            most_decks = card.second.deck_count;
//    }
//
//    // TODO lower case all card names?
//    std::cerr << "done." << std::endl;

    std::cerr << "Loading EDHREC data from " + path + "/EDHREC.json ...";
    std::ifstream ifs(path + "/EDHREC.json");
    nlohmann::json edhrec_json;
    ifs >> edhrec_json;
    ifs.close();

    edhrec_data = edhrec_json.get<decltype(edhrec_data)>();

    // calculate individual values. Find:
    // most expensive card
    // lowest ranked card
    // most number of decks
    for (const auto card : edhrec_data)
    {
        if (card.second.rank > lowest_rank)
        {
            lowest_rank = card.second.rank;
        }
        if (card.second.price > highest_price)
        {
            highest_price = card.second.price;
        }
        if (card.second.deck_count > most_decks)
        {
            most_decks = card.second.deck_count;
        }
    }

    std::cerr << "done." << std::endl;
}

evaluation edhrec_price(const card *card, const uint16_t index, const card::format format)
{
    if (edhrec_data.count(card->name))
    {
        return {edhrec_data[card->name].price, highest_price, "edhrec price"};
    }

    return {0.0, highest_price, "edhrec price"};
}

evaluation edhrec_rank(const card *card, const uint16_t index, const card::format format)
{
    if (edhrec_data.count(card->name))
    {
        return {(double)(lowest_rank - edhrec_data[card->name].rank), (double)lowest_rank, "edhrec rank"};
    }

    return {0.0, (double)lowest_rank, "edhrec rank"};
}

evaluation edhrec_decks(const card *card, const uint16_t index, const card::format format)
{
    if (edhrec_data.count(card->name))
    {
        return {(double)(edhrec_data[card->name].deck_count), (double)most_decks, "edhrec decks"};
    }

    return {0.0, (double)most_decks, "edhrec decks"};
}

evaluation edhrec_synergy(const card *card_a, const card *card_b, const card::format format)
{
    // TODO eliminate this case
    if (card_a->name == card_b->name)
    {
        return {0.0, 1.0, "edhrec synergy"};
    }

    if (edhrec_data.count(card_a->name) && edhrec_data[card_a->name].synergies.count(card_b->name))
    {
        auto rank = edhrec_data[card_a->name].synergies[card_b->name];
        //rank ranges from -1.0 to 1.0
        if (rank < 0)
        {
            rank = 0;
        }
        return {rank, 1.0, "edhrec synergy"};
    }

    return {0.0, 1.0, "edhrec synergy"};
}


void from_json(const nlohmann::json &j, edhrec_item &c)
{
    if (!j.is_object())
    {
        return; // TODO throw!
    }

    try
    {
        c.deck_count = j["deck_count"].get<uint>();
    }
    catch (...)
    {
        c.deck_count = 0;
    }

    try
    {
        c.rank = j["edhrec_rank"].get<uint>();
    }
    catch (...)
    {
        c.rank = lowest_rank;
    }

    try
    {

        c.price = j["price"].get<double>();
    }
    catch (...)
    {
        c.price = 0.0;
    }

//    try
//    {
    if (!j["synergies"].is_object())
    {
        std::cerr << j.dump() << std::endl;
    }
    c.synergies = j["synergies"].get<decltype(c.synergies)>();
//    }
//    catch (nlohmann::json::out_of_range &e)
//    {}

    return;
}

}
}