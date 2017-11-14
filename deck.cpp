//
// Created by Don Goodman-Wilson on 14/11/2017.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>

#include "deck.h"

// TODO need a legality object to capture the context of legality, including deck size!

namespace magique
{

deck::deck(const std::string &filename, const catalog &catalog) : legal_{true}, rank_{0.0}, colors_{0}
{
    for (int i = cost_dist_.size() - 1; i >= 0; --i)
    {
        cost_dist_[i] = 0;
    }

    type_dist_[card::type::basic_land] = 0;
    type_dist_[card::type::land] = 0;
    type_dist_[card::type::creature] = 0;
    type_dist_[card::type::artifact] = 0;
    type_dist_[card::type::enchantment] = 0;
    type_dist_[card::type::planeswalker] = 0;
    type_dist_[card::type::instant] = 0;
    type_dist_[card::type::sorcery] = 0;


    std::ifstream file{filename};

    std::string row;
    while (getline(file, row))
    {
//        std::cout << row << std::endl;
        if (row[0] == '\\') continue;
        std::istringstream tokenS(row);
        std::string count_str;
        std::string name;
        getline(tokenS, count_str, ' '); //get the count
        getline(tokenS, name); //get the name

        auto count = stoul(count_str);
        for (auto i = count; i > 0; --i)
        {
            card c = catalog.at(name);
            cards_.emplace_back(c);
        }
    }
}

deck::deck(const std::vector<uint64_t> &indices, const collection &collection) : legal_{true}, rank_{0.0}, colors_{0}
{
    for (int i = cost_dist_.size() - 1; i >= 0; --i)
    {
        cost_dist_[i] = 0;
    }

    type_dist_[card::type::basic_land] = 0;
    type_dist_[card::type::land] = 0;
    type_dist_[card::type::creature] = 0;
    type_dist_[card::type::artifact] = 0;
    type_dist_[card::type::enchantment] = 0;
    type_dist_[card::type::planeswalker] = 0;
    type_dist_[card::type::instant] = 0;
    type_dist_[card::type::sorcery] = 0;

    for(const auto &i : indices)
    {
        cards_.emplace_back(collection.at(i));
    }
}

double deck::eval()
{
    // TODO remove cards from deck that appear more times than in the collection!!

    //LEGALITIES!

    // count the size of the deck. For now assume a deck size of 30, with 12 land
    int64_t count_diff = cards_.size() - (30-12);
    if (count_diff < 0)
    { legal_ = false; }
    else
    { count_diff *= -1; }
    rank_ += count_diff;
    reasons_["count_diff"] = count_diff;

    std::map<std::string, uint8_t> dupes;

    // count for duplicates, ignoring basic land
    std::vector<card> deck_sans_basic_land;
    uint8_t dupe_count = 0;
    for (const auto &card : cards_)
    {
        bool land{false};
        for (const auto &type : card.types)
        {
            if (type == card::type::basic_land)
            {
                land = true;
                break;
            }
        }
        if (land) continue;

        dupes[card.name]++;
        if (dupes[card.name] > 4)
        {
            legal_ = false;
            ++dupe_count;
        }
    }

    rank_ -= dupe_count; // severe penalty for each dupe
    reasons_["dupe_count"] = dupe_count;



    // basic rankings; only affect ranking if the deck is legal, but there are things we want to computer anyway for diagnostics

    // from this point, eliminate and lands from our calculation;

    std::vector<card> deck_sans_land;
    for (const auto &card : cards_)
    {
        bool land{false};
        for (const auto &type : card.types)
        {
            if (type == card::type::land)
            {
                land = true;
                break;
            }
        }
        if (land) continue;

        deck_sans_land.emplace_back(card);
    }

    std::unordered_set<card::color> colors_seen;
    //count mana cost distribution
    for (const auto &card : deck_sans_land)
    {
        // mana cost distribution
        cost_dist_[card.converted_mana_cost]++;

        // color distribution
        for (const auto &color : card.color_identity)
        {
            if (colors_seen.count(color) == 0)
            {
                colors_seen.insert(color);
            }
        }

        //type distribution
        for (const auto &type : card.types)
        {
            type_dist_[type]++;
        }
    }
    colors_ = colors_seen.size();


    //calculate deviation from ideal mana color distribution
    if (legal_)
    {
        double bonus;
        switch (colors_)
        {
            case 2:
                bonus = deck_sans_land.size() / 6.0;  // 1/6 card bonus for 2 colors
                break;
            case 1:
            case 3:
                bonus = -1;
                break;
            case 4:
                bonus = -4;
                break;
            case 5:
                bonus = -8;
                break;
            case 6:
                bonus = -15;
                break;
        }
        rank_ += bonus;
        reasons_["colors"] = bonus;
        reasons_["colors_seen"] = colors_seen.size();
    }


    //calculate difference from ideal mana cost distribution
    // TODO this should be something specified, and depends upon the deck size
    std::array<double, 11> ideal_cmc{0.05, 0.09, 0.10, 0.18, 0.24, 0.18, 0.09, 0.05, 0.01, 0.01, 0.00};
    float cmc_distance{0.0};
    for (int i = cost_dist_.size() - 1; i >= 0; --i)
    {
        auto x = (double) cost_dist_[i];
        auto y = ideal_cmc[i] * deck_sans_land.size();
        cmc_distance += x * x - y * y;
    }
    cmc_distance = sqrt(cmc_distance);

    if (legal_)
    {
        auto distance = (deck_sans_land.size() / 2.0 - cmc_distance);
        rank_ += distance;
        reasons_["cmc_distance"] = distance;

    }

    // type distribution //TODO make this configurable!
    std::map<card::type, double> ideal_types{
            {card::type::creature,     0.600},
            {card::type::artifact,     0.0333},
            {card::type::enchantment,  0.10},
            {card::type::planeswalker, 0.0333},
            {card::type::instant,      0.13333},
            {card::type::sorcery,      0.10}
    };
    float type_distance{0.0};
    for (const auto &type : type_dist_)
    {
        // skip land
        if(type.first == card::type::land || type.first == card::type::basic_land) continue;
        auto x = (double) type_dist_.at(type.first);
        auto y = ideal_types.at(type.first) * deck_sans_land.size();
        type_distance += x * x - y * y;
    }
    type_distance = sqrt(cmc_distance);

    if (legal_)
    {
        auto distance = (deck_sans_land.size() / 2.0 - cmc_distance);
        rank_ += distance;
        reasons_["type_distance"] = distance;

    }

    return rank_;
}

void to_json(nlohmann::json &j, const deck &d)
{
    j["legal"] = d.legal_;
    j["rank"] = d.rank_;
    j["deck_size"] = d.cards_.size();
    j["mana_distribution"] = d.cost_dist_;

    j["type_distribution"] = nlohmann::json{
            {"basic_land",   d.type_dist_.at(card::type::basic_land)},
            {"land",         d.type_dist_.at(card::type::land)},
            {"creature",     d.type_dist_.at(card::type::creature)},
            {"artifact",     d.type_dist_.at(card::type::artifact)},
            {"enchantment",  d.type_dist_.at(card::type::enchantment)},
            {"planeswalker", d.type_dist_.at(card::type::planeswalker)},
            {"instant",      d.type_dist_.at(card::type::instant)},
            {"sorcery",      d.type_dist_.at(card::type::sorcery)}
    };

    std::vector<std::string> cards;
    for (const auto &card : d.cards_)
    {
        cards.push_back(card.name);
    }
    std::sort(cards.begin(), cards.end());
    j["cards"] = cards;
    j["reasons"] = d.reasons_;
}

void from_json(const nlohmann::json &j, deck &p)
{
//TODO
}


} //namespace magique