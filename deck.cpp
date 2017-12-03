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

std::vector<card> deck::key_cards_ = {};

deck::deck(const std::string &filename, const catalog &catalog, interactions interactions) : interactions_{interactions}, legal_{true}, rank_{0.0}, colors_{0}
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
            bool land{false};
            for (const auto &t:c.types)
            {
                if (t == card::type::land || t == card::type::basic_land)
                {
                    land = true;
                    break;
                }
            }
            if (!land)
            {
                cards_.emplace_back(c);
            }
        }
    }
}

deck::deck(const std::vector<uint64_t> &indices, const collection &collection, interactions interactions) : interactions_{interactions},  legal_{true}, rank_{0.0}, colors_{0}
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


    // remove any elements that appear twice
    std::unordered_multiset<uint64_t> collection_duplicates;

    //add key cards first
    for (const auto &card :key_cards_)
    {
        auto index = collection.index_at(card.name);
        collection_duplicates.insert(index);
        cards_.emplace_back(card);
    }

    for (const auto &i : indices)
    {
        collection_duplicates.insert(i);
        if (collection_duplicates.count(i) == 1)
        {
            cards_.emplace_back(collection.at(i));
        }
    }
}

double deck::evaluate()
{
    rank_ = 30;

    //LEGALITIES!

    // count the size of the deck. For now assume a deck size of 30, with 12 land
    int64_t count_diff = cards_.size() - (30 - 12);
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


    // basic rankings; only affect ranking if the deck is legal, but there are things we want to computer anyway for diagnostics

    // from this point, eliminate and lands from our calculation;
    std::map<card::color, uint8_t> colors_seen;

    double power{0};
    double toughness{0};

    uint8_t colored_card_count_{0};

    std::map<uint16_t, double> interaction_scores;

    for (const auto &card : cards_)
    {
        dupes[card.name]++;
        if (dupes[card.name] > 4)
        {
            legal_ = false;
            ++dupe_count;
        }

        // mana cost distribution
        cost_dist_[card.converted_mana_cost]++;

        // color distribution
        if (card.color_identity.size() != 0)
        {
            colored_card_count_++;
        }
        for (const auto &color : card.color_identity)
        {
            colors_seen[color]++;
        }

        //type distribution
        for (const auto &type : card.types)
        {
            type_dist_[type]++;
        }


        //fun stuff
        //lets tally the str and tuf
        if (card.power)
        {
            uint8_t pow;
            if (*card.power == "*")
            {
                pow = 5;
            }
            else
            { pow = stoul(*card.power); }
            power += pow;
        }
        if (card.toughness)
        {
            uint8_t tuf;
            if (*card.toughness == "*")
            {
                tuf = 5;
            }
            else
            { tuf = stoul(*card.toughness); }
            toughness += tuf;
        }

        // interactions!
        for(auto i = 0; i < cards_.size(); ++i)
        {
            interaction_scores[i] = 0;

            for(auto j = 0; j < cards_.size(); ++j)
            {
                if(i==j) continue; // don't compare a card to itself

                interaction_scores[i] += interactions_.evaluate(cards_[i], cards_[j]);
            }
        }

        // TODO
        // minomize casting cost (but mana curve?) relative to strength?
    }


    // TODO this is all fucked up
    double interactions_bonus{0};
    reasons_["interactions"] = nlohmann::json{};
    for(const auto &score : interaction_scores)
    {
        interactions_bonus += score.second;
        reasons_["interactions"][score.first] = score.second;
    }
    rank_ += interactions_bonus;

    rank_ -= dupe_count; // severe penalty for each dupe
    reasons_["dupe_count"] = dupe_count;

    //count mana cost distribution
    colors_ = colors_seen.size();

    //calculate color balance. Ideally (maybe?) all colors are equally distributed.
    auto ideal_color_count = colored_card_count_ / colors_; // TODO this doesn't account for colorless cards!
    double color_balance_adjustment_ = 0;
    for (const auto &c: colors_seen)
    {
        color_balance_adjustment_ += abs(ideal_color_count - c.second);
    }
    rank_ -= color_balance_adjustment_; //TODO weight?
    reasons_["color_balance_adjustment"] = color_balance_adjustment_;


    //calculate deviation from ideal mana color distribution
    double bonus;
    switch (colors_)
    {
        case 2:
            bonus = 0;  // 1/6 card bonus for 2 colors
            break;
        case 1:
        case 3:
            bonus = -8;
            break;
        case 4:
            bonus = -16;
            break;
        case 5:
        default:
            bonus = -32;
            break;
    }
    rank_ += bonus;
    reasons_["color_bonus"] = bonus;
    reasons_["colors_seen"] = colors_seen.size();


    //calculate difference from ideal mana cost distribution
    // TODO this should be something specified, and depends upon the deck size
    std::array<double, 11> ideal_cmc{0.09, 0.09, 0.10, 0.18, 0.20, 0.18, 0.09, 0.05, 0.01, 0.01, 0.00};
    double cmc_distance{0.0};
    for (int i = cost_dist_.size() - 1; i >= 0; --i)
    {
        auto x = (double) cost_dist_[i];
        auto y = ideal_cmc[i] * cards_.size();
        cmc_distance += (x - y) * (x - y);
    }
    cmc_distance = sqrt(cmc_distance);


    auto cmc_distance_score = (cards_.size() / 2.0 - cmc_distance);
    rank_ += cmc_distance_score;
    reasons_["cmc_distance"] = cmc_distance;
    reasons_["cmc_distance_score"] = cmc_distance_score;


    // type distribution //TODO make this configurable!
    std::map<card::type, double> ideal_types{
            {card::type::creature,     0.600},
            {card::type::artifact,     0.0333},
            {card::type::enchantment,  0.10},
            {card::type::planeswalker, 0.0333},
            {card::type::instant,      0.13333},
            {card::type::sorcery,      0.10}
    };
    double type_distance{0.0};
    for (const auto &type : type_dist_)
    {
        // skip land
        if (type.first == card::type::land || type.first == card::type::basic_land) continue;
        auto x = (double) type_dist_.at(type.first);
        auto y = ideal_types.at(type.first) * cards_.size();
        type_distance += (x - y) * (x - y);
    }
    type_distance = sqrt(type_distance);

    auto type_distance_score = (cards_.size() / 2.0 - type_distance);
    rank_ += type_distance_score;
    reasons_["type_distance"] = type_distance;
    reasons_["type_distance_score"] = type_distance_score;

    if (!legal_)
    {
        rank_ -= 10;
    }

    if (rank_ < 0) rank_ = 0;


    //// FUN STUFF!

    auto avg_pow = power / cards_.size();
    reasons_["tot_pwr"] = power;
    reasons_["avg_pwr"] = avg_pow;
    rank_ += avg_pow;

    auto avg_tuf = toughness / cards_.size();
    reasons_["tot_tuf"] = toughness;
    reasons_["avg_tuf"] = avg_tuf;
    rank_ += avg_tuf;

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

//    std::vector<std::string> cards;
//    for (const auto &card : d.cards_)
//    {
//        cards.push_back(card.name);
//    }
//    std::sort(cards.begin(), cards.end());
    j["cards"] = d.cards_;
    j["reasons"] = d.reasons_;
}

void from_json(const nlohmann::json &j, deck &p)
{
//TODO
}


} //namespace magique