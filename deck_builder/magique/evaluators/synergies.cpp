//
// Created by Donald Goodman-Wilson on 16/09/2018.
//

#include "synergies.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <shared_mutex>

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

static std::unordered_map<std::string, std::vector<uint16_t>> mechanics_;
static std::unordered_map<std::string, std::unordered_map<std::string, double>> conditional_probabilities_cards_;
static std::unordered_map<uint16_t, std::unordered_map<uint16_t, double>> conditional_probabilities_mechanics_;
static std::unordered_map<std::string, std::unordered_map<std::string, double>> conditional_probabilities_card_mechanics_cache_;
static std::unordered_map<std::string, std::set<std::string>> tribal_synergies_;

static std::shared_mutex cache_mutex_;

void load_synergies(std::string path, magique::catalog &catalog, magique::card::format format)
{
    if ((format == magique::card::format::duel) || (format == magique::card::format::onevone) ||
        (format == magique::card::format::brawl) || (format == magique::card::format::penny))
    {
        std::cerr << "No mtgdecks data for format " << magique::to_string(format) << std::endl;
        return;
    }

    have_data = true;
    std::cerr << "Loading tag data...";

    {
        std::ifstream ifs{path + "/synergies/card_mechanics.json"};
        nlohmann::json mechanics_json;
        ifs >> mechanics_json;
        mechanics_ = mechanics_json.get<decltype(mechanics_)>();
        ifs.close();
    }

    {
        std::ifstream ifs{path + "/synergies/conditional_probabilities_card_" + to_string(format) + ".json"};
        nlohmann::json conditional_probabilities_cards_json;
        ifs >> conditional_probabilities_cards_json;
        conditional_probabilities_cards_ = conditional_probabilities_cards_json.get<decltype(conditional_probabilities_cards_)>();
        ifs.close();
    }

    {
        // TODO do we need to convert the keys from strings to ints?
        std::ifstream ifs{path + "/synergies/conditional_probabilities_mechanic_" + to_string(format) + ".json"};
        nlohmann::json conditional_probabilities_mechanics_json;
        ifs >> conditional_probabilities_mechanics_json;
        for (auto &kv : conditional_probabilities_mechanics_json.items())
        {
            uint16_t key1 = std::stoi(kv.key());
            for (auto &kv2 : kv.value().items())
            {
                uint16_t key2 = std::stoi(kv2.key());
                conditional_probabilities_mechanics_[key1][key2] = kv2.value().get<double>();
            }
        }
        ifs.close();
    }

    {
        std::ifstream ifs{path + "/synergies/card_tribes.json"};
        nlohmann::json tribal_synergies_json;
        ifs >> tribal_synergies_json;
        for (auto &kv : tribal_synergies_json.items())
        {
            std::string card_name{kv.key()};
            for (auto &kv2 : kv.value().items())
            {
                if (kv2.key() == "subtypes")
                {
                    for (const auto &subtype : kv2.value())
                    {
                        catalog.at(card_name)->subtypes.emplace(subtype.get<std::string>());
                    }
                }
                else if (kv2.key() == "tribes")
                {
                    for (const auto &name : kv2.value())
                    {
                        tribal_synergies_[card_name].emplace(name.get<std::string>());
                    }
                }
            }
        }

        ifs.close();
    }

    std::cerr << "done." << std::endl;
}

evaluation card_synergy(const card *card_a, const card *card_b, card::format format)
{
    // TODO DOn't make me do this!
    // TODO eliminate this case
    if (card_a->name == card_b->name)
    {
        return {0.0, 1.0, "card synergy"};
    }

    if (!have_data)
    {
        return {0.0, 1.0, "SYNERGY DATA NOT LOADED"};
    }

    if (conditional_probabilities_cards_.count(card_a->name))
    {
        if (conditional_probabilities_cards_.at(card_a->name).count(card_b->name))
        {
            return {conditional_probabilities_cards_.at(card_a->name).at(card_b->name), 1.0, "card synergy"};
        }
    }

    return {0.0, 1.0, "card synergy"};
}

evaluation mechanic_synergy(const card *card_a, const card *card_b, card::format format)
{
    // TODO eliminate this case
    if (card_a->name == card_b->name)
    {
        return {0.0, 1.0, "card synergy"};
    }

    double synergy{0.0};

    bool cached{false};
    {
        std::shared_lock<std::shared_mutex> l{cache_mutex_};
        if (conditional_probabilities_card_mechanics_cache_.count(card_a->name) &&
            conditional_probabilities_card_mechanics_cache_.at(card_a->name).count(card_b->name))
        {
            //pull from cache
            synergy = conditional_probabilities_card_mechanics_cache_.at(card_a->name).at(card_b->name);
            cached = true;
        }
    }

    if (!cached)
    {
        // Not that the scale will vary from card to card. We don't want to do a division each time! But the upper limit
        // For a particular card paring is the number of mechanics they have in common.

        const auto mechanics_a_count = mechanics_.count(card_a->name);
        const auto mechanics_b_count = mechanics_.count(card_b->name);
        if (mechanics_a_count == 0 || mechanics_b_count == 0)
        {
            return {0.0, 1.0, "mechanic synergy"};
        }

        const auto &mechanics_a = mechanics_.at(card_a->name);
        const auto &mechanics_b = mechanics_.at(card_b->name);

        const auto divisor = mechanics_a.size() * mechanics_b.size();


        for (const auto &mech_a : mechanics_a)
        {
            for (const auto &mech_b : mechanics_b)
            {
                if (conditional_probabilities_mechanics_.count(mech_a))
                {
                    if (conditional_probabilities_mechanics_.at(mech_a).count(mech_b))
                    {
                        auto a = conditional_probabilities_mechanics_.at(mech_a).at(mech_b);
                        synergy += a;
                    }
                }

            }
        }

        // I'd much rather not do a division here. There must be a better way. But I can't think of one. So let's try cacheing
        synergy = synergy / divisor;
        // Let's only do this division once. By cacheing the results
        // Not thread safe
        {
            std::unique_lock<std::shared_mutex> l{cache_mutex_};
            conditional_probabilities_card_mechanics_cache_[card_a->name][card_b->name] = synergy;
        }
    }

    // TODO why do we even return a divisor here? We should return it as part of the loading process, I think.
    return {synergy, 1.0, "mechanic synergy"};
}

evaluation tribal_synergy(const card *card_a, const card *card_b, card::format format)
{
    // TODO eliminate this case
    if (card_a->name == card_b->name)
    {
        return {0.0, 1.0, "card synergy"};
    }

    // static std::unordered_map<std::string, std::vector<std::string>> tribal_synergies_;
    if (tribal_synergies_.count(card_a->name))
    {
        // check tribal synergies of A against subtypes in B
        for (const auto &tribe : tribal_synergies_.at(card_a->name))
        {
            if (card_b->subtypes.count(tribe))
            {
                return {1.0, 1.0, "tribal synergy"};
            }
        }

        // check tribal synergies of A against tribal synergies in B
        if (tribal_synergies_.count(card_b->name))
        {
            for (const auto &tribe_a : tribal_synergies_.at(card_a->name))
            {
                for (const auto &tribe_b : tribal_synergies_.at(card_a->name))
                {
                    if (tribe_a == tribe_b)
                    {
                        return {0.5, 1.0, "tribal synergy"};
                    }
                }
            }
        }
    }
    return {0.0, 1.0, "tribal synergy"};
}

}
}