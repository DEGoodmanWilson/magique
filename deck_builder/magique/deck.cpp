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

uint16_t deck::colors{2};
uint16_t deck::deck_minimum{60 - 26};
std::set<card::color> deck::color_identity{};

std::vector<card> deck::key_cards_ = {};

deck::deck(const std::string &filename, const catalog &catalog, const interactions &interactions) :
        interactions_{interactions}, rank_{0.0}
{
//    for (int i = cost_dist_.size() - 1; i >= 0; --i)
//    {
//        cost_dist_[i] = 0;
//    }
//
//    type_dist_[card::type::basic_land] = 0;
//    type_dist_[card::type::land] = 0;
//    type_dist_[card::type::creature] = 0;
//    type_dist_[card::type::artifact] = 0;
//    type_dist_[card::type::enchantment] = 0;
//    type_dist_[card::type::planeswalker] = 0;
//    type_dist_[card::type::instant] = 0;
//    type_dist_[card::type::sorcery] = 0;
//

    std::ifstream file{filename};

    std::string row;
    while (getline(file, row))
    {
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
    deck_minimum = cards_.size();
}

deck::deck(const std::vector<uint64_t> &indices, const collection &collection, const interactions &interactions) :
        interactions_{interactions}, rank_{0.0}
{


    // In previous versions of this function, I used step functions for evaluating legalities and basic deck dynamics.
    // However, although this worked fine in 30-card decks, this wasn't fine-grained enough to work for 60-card decks.
    // So, the thing to remember is: Always allow the GA to climb hills in small steps, never require large leaps
    // to succeed. First, sometimes, oftentimes, those big leaps are never made (and when they are it is only by
    // accident!), and second, when that leap is accomplished, the resulting gene becomes superfit, and ends up
    // dominating as a local maximum. So allow it to hillclimb step by step.

    // TODO at the moment, we are assuming that there are NO basic land cards in this deck, that those will be added
    // _post-GA_, because a GA isn't going to be a lot of help in selecting basic lands to use.

    // The idea is that we'll rate a deck _per card_. So a good deck will have, let us say, 1 point per card.

    // FIRST: LEGALITIES!


    // SECOND: BASIC DYNAMICS!

    // TODO collect these stats during contruction, maybe? To speed things up?

    // There is an ideal—or perhaps several ideal—distributions of mana costs. You don't want too many 1-CMC or 7-CMC cards in your hand!
    std::array<uint8_t, 11> cost_dist{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // Same with the type distribution. You want a mix of creatures, instants, etc.
    std::multiset<card::type> type_dist;

    // A good deck has a high average power and toughness
    double power{0};
    double toughness{0};

    // THIRD: Card interactions!

    // A good deck has cards that work _together_. Here are the scores for each card.
    std::vector<double> interaction_scores;


    // FOURTH: Other shit.

    // A counter…we'll use it below.
    uint16_t i{0};

    // This is a handy diagnostic thing. Each time we modify the evaluation, we record it here so we can dump the diagnostics as JSON later
    reasons_.clear();

    std::vector<card> expanded_deck;

    // remove any elements that appear twice
    std::unordered_multiset<uint64_t> collection_duplicates;
    // remove any non-land cards that appear more than 4 times
    std::multiset<std::string> dupes;
    // remove any cards that are not among the top N colors
    std::unordered_map<card::color, uint16_t> colors_seen{
            {card::color::white,     0},
            {card::color::blue,      0},
            {card::color::black,     0},
            {card::color::red,       0},
            {card::color::green,     0},
            {card::color::colorless, 0}
    };

    // TODO this code is super tortured. It's pretty optimized is part of the reason why.
    // What we are attempting to do, all in one go is:
    // Cull any attempt to include the same physical card more than once
    // Cull any duplicates beyond the legal limits of 4
    // Establish what the n most dominant colors are in the deck, and cull any cards that do
    //  not share in those color ids.
    // I think this can be cleaned up significantly! It certainly needs to be refactored
    //  to allow the key cards to establish the deck color identity, and only fall back on the most
    //  numerous colors represented in the absence of a key card.

    std::unordered_set<card::color> top_colors;
    bool mandated_color_identity{false};

    //add key cards first. If there are key cards, use their color identity as the prefered color identity
    std::unordered_map<std::string, int64_t> indices_seen;
    for (const auto &card :key_cards_)
    {
        mandated_color_identity = true;
        int64_t last_index{0};
        if (indices_seen.count(card.name)) last_index = indices_seen[card.name];
        auto index = collection.index_at(card.name, last_index);
        indices_seen[card.name] = index;
        collection_duplicates.insert(index); // TODO handle multiple key cards of same name!

        dupes.insert(card.name);
        if (dupes.count(card.name) > 4) continue; //skip 5th+ card of same name

        for (const auto &color : card.color_identity)
        {
            top_colors.insert(color);
        }

        expanded_deck.emplace_back(card);
    }

    // see if a color identity was mandated. If so, add those colors
    if(color_identity.size() > 0)
    {
        mandated_color_identity = true;
        for(const auto &color : color_identity)
        {
            top_colors.insert(color);
        }
    }


    for (const auto &i : indices)
    {
        collection_duplicates.insert(i);
        if (collection_duplicates.count(i) > 1) continue; // skip dupe references to same physical card

        auto card = collection.at(i);
        dupes.insert(card.name);
        if (dupes.count(card.name) > 4) continue; //skip 5th+ card of same name

        if (!mandated_color_identity)
        {
            for (const auto &color : card.color_identity)
            {
                colors_seen[color]++;
            }
        }
        expanded_deck.emplace_back(card);
    }

    // identify the top N colors
    if (!mandated_color_identity)
    {
        std::vector<std::pair<card::color, uint16_t>> sorted_colors;
        for (auto color : card::all_colors)
        {
            if (color == card::color::colorless) continue;

            sorted_colors.emplace_back(std::make_pair(color, colors_seen[color]));
        }
        std::sort(sorted_colors.begin(), sorted_colors.end(), [](const auto &first, const auto &second)
        {
            return std::get<uint16_t>(first) > std::get<uint16_t>(second);
        });


        for (auto i = 0; i < colors; ++i)
        {
            top_colors.insert(std::get<card::color>(sorted_colors[i]));
        }
    }

    reasons_["colors"] = top_colors;

    // Remove any cards not among the top N colors.
    for (const auto &card : expanded_deck)
    {
        bool insert{true};
        for (const auto &color : card.color_identity)
        {
            if (top_colors.count(color) == 0)
            {
                insert = false;
                break;
            }
        }
        if (insert)
        {
            cards_.emplace_back(card);

            // update mana cost distribution
            cost_dist[card.converted_mana_cost]++;

            // update type distribution
            for (const auto &type : card.types)
            {
                type_dist.insert(type);
            }


            // lets tally the str and tuf
            if (card.power)
            {
                if (*card.power == "*")
                {
                    power += 3; // because why not. We don't know what the power actually is, but ideally we can get the card pretty strong.
                }
                else
                {
                    power += stoul(*card.power);
                }
            }
            if (card.toughness)
            {
                if (*card.toughness == "*")
                {
                    toughness += 3;
                }
                else
                {
                    toughness += stoul(*card.toughness);
                }
            }


            // interactions!
            double interaction_score = 0;
            for (auto j = 0; j < cards_.size(); ++j)
            {
                if (i == j) continue; // don't compare a card to itself. This is why we're keeping track of i

                interaction_score += interactions_.evaluate(card, cards_[j]);
            }
            interaction_score /= 100000;
            interaction_score /= cards_.size();
            if (card.bonus_multiplier != 1.0)
            {
                interaction_score *= card.bonus_multiplier;
            }
            // if a card interacts with every other card in a deck, it will be worth 1 point, regardless of deck size
            interaction_scores.push_back(interaction_score);
            reasons_["interaction_scores"][card.name] = interaction_score;

            ++i;
        }


        // Now that we've gone through each card, we can now think of the deck as a whole.

        rank_ = 0;

        // penalty for being too small
        if (cards_.size() < deck_minimum)
        {
            rank_ -= 2 * (deck_minimum - cards_.size());
        }

        // Now let's compute the distance from the ideal CMC distribution
        // TODO this should be something specified by the user
        // for now use the turn-4 format from https://www.channelfireball.com/articles/frank-analysis-finding-the-optimal-mana-curve-via-computer-simulation/
        std::array<double, 11> ideal_cmc_distribution{0, 9, 13, 9, 3, 0, 0, 0, 0, 0, 0};
        // normalize
        auto sum = std::accumulate(ideal_cmc_distribution.begin(), ideal_cmc_distribution.end(), 0);
        for (auto &item : ideal_cmc_distribution)
        {
            item = item / sum;
        }

        double cmc_distance{0.0};
        for (int i = cost_dist.size() - 1; i > 0; --i) // don't count zero-CMC cards
        {
            auto x = (double) cost_dist[i];
            auto y = ideal_cmc_distribution[i] * cards_.size();
            cmc_distance += (x - y) * (x - y);
        }

        cmc_distance = sqrt(cmc_distance);
        rank_ -= cmc_distance;
        reasons_["cmc_distance"] = cmc_distance;
        reasons_["cmc"] = cost_dist;



        // type distribution
        // TODO make this configurable!
        std::unordered_map<card::type, double> ideal_type_distribution{
                {card::type::creature,     16},
                {card::type::artifact,     8},
                {card::type::enchantment,  3},
                {card::type::planeswalker, 1},
                {card::type::instant,      3},
                {card::type::sorcery,      3}
        };
        //normalize
        auto type_sum = std::accumulate(ideal_type_distribution.begin(), ideal_type_distribution.end(), 0,
                                        [](auto value, const auto &p) -> auto
                                        {
                                            return value + p.second;
                                        }
        );
        for (auto &item : ideal_type_distribution)
        {
            item.second = item.second / type_sum;
        }

        double type_distance{0.0};
        for (const auto &type : card::all_types)
        {
            // skip land
            if (type == card::type::land || type == card::type::basic_land) continue;
            auto x = (double) type_dist.count(type);
            auto y = ideal_type_distribution.at(type) * cards_.size();
            type_distance += (x - y) * (x - y);
        }
        type_distance = sqrt(type_distance);

        rank_ -= type_distance;
        reasons_["type_distance"] = type_distance;
        reasons_["type_distribution"] = type_dist;


        // Average power and toughness
        auto avg_pow = power / cards_.size();
        reasons_["tot_pwr"] = power;
        reasons_["avg_pwr"] = avg_pow;
        rank_ += avg_pow;

        auto avg_tuf = toughness / cards_.size();
        reasons_["tot_tuf"] = toughness;
        reasons_["avg_tuf"] = avg_tuf;
        rank_ += avg_tuf;


        // Ooooh interactions, sweet!
        double interactions_bonus{0};
        i = 0;
        for (const auto &score : interaction_scores)
        {
            interactions_bonus += score;
            ++i;
        }
        rank_ += interactions_bonus;
        reasons_["interactions_bonus"] = interactions_bonus;
    }
}

void to_json(nlohmann::json &j, const deck &d)
{

    j["_list"] = nlohmann::json::array();
    for (const auto &card: d.cards_)
    {
        j["_list"].push_back(card.name);
    }
    std::sort(j["_list"].begin(), j["_list"].end());
    j["_count"] = d.cards_.size();

    j["rank"] = d.rank_;
    j["cards"] = d.cards_;
    j["reasons"] = d.reasons_;
}

void from_json(const nlohmann::json &j, deck &p)
{
//TODO
}


} //namespace magique