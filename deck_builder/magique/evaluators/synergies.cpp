//
// Created by Donald Goodman-Wilson on 16/09/2018.
//

#include "synergies.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

namespace magique
{
namespace evaluators
{

const std::vector<std::string> explode_(const std::string &s, const char &c)
{
    std::string buff{""};
    std::vector<std::string> v;

    for (auto n:s)
    {
        if (n != c)
        { buff += n; }
        else if (n == c && buff != "")
        {
            v.push_back(buff);
            buff = "";
        }
    }
    if (buff != "") v.push_back(buff);

    return v;
}

static bool have_data{false};

static std::unordered_map<std::string, std::vector<uint64_t>> mechanics;

static std::array<std::array<double, 250>, 250> interactions_store_; // TODO hard-coded number of classes, ugh.
static std::unordered_map<std::string, std::vector<std::string>> synergies_store_; // TODO hard-coded number of classes, ugh.

void load_synergies(std::string path, const magique::catalog &catalog, magique::card::format format)
{
    if ((format == magique::card::format::duel) || (format == magique::card::format::onevone) ||
        (format == magique::card::format::brawl) || (format == magique::card::format::penny))
    {
        std::cerr << "No mtgdecks data for format " << magique::to_string(format) << std::endl;
        return;
    }

    have_data = true;

    std::cerr << "Loading tag data...";
    std::ifstream ifs{path + "/synergies/card_tags.json"};
    nlohmann::json mechanics_json;
    ifs >> mechanics_json;
    ifs.close();

    // AllJson from MTGJSON is one large object
    for (const auto& card_kv : catalog)
    {
        auto name = card_kv.first;
        auto card_json = card_kv.second;
        //load its annotations, if any
        try
        {
            mechanics[name] = mechanics_json[name].get<std::vector<uint64_t>>();
        }
        catch (nlohmann::json::out_of_range &e)
        {}
    }


    ifs = std::ifstream{path + "/synergies/conditional_probabilities_" + to_string(format) + ".json"};
    nlohmann::json interactions;
    ifs >> interactions;
    ifs.close();

    ifs = std::ifstream{path + "/synergies/card_synergies.json"};
    nlohmann::json synergies;
    ifs >> synergies;
    ifs.close();

    // ok what we have is an object of interactions. The key is a pair of numbers, the keys into our map.
    // TODO make this format easier to parse!
//    for (auto it = interactions.begin(); it != interactions.end(); ++it)
//    {
//        //key is a string of two ints with a |, e.g. "1|2", value is a double.
//        auto keys = explode_(it.key(), '|');
//        auto m = std::atoi(keys[0].c_str());
//        auto n = std::atoi(keys[1].c_str());
//
//        interactions_store_[m][n] = it.value().get<double>();
//    }
//
//    synergies_store_ = synergies.get<decltype(synergies_store_)>();

    std::cerr << "done." << std::endl;
}

evaluation synergies(const card *card_a, const card *card_b, const card::format format)
{
    if(!have_data)
    {
        return {0.0, 1.0, "synergies"};
    }

    // TODO
    return {1.0, 1.0, "synergies"};
}

}
}