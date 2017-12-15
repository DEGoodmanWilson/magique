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

deck::deck(const std::string &filename, const catalog &catalog, interactions interactions) :
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
}

deck::deck(const std::vector<uint64_t> &indices, const collection &collection, interactions interactions) :
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


    // the deck size is 60 - 24 (basic lands). ANything less is illegal TODO THIS MUST BE USER CONFIGURABLE!
    uint8_t legal_deck_minimum{60-24};


    // Regardless of deck size, or anything else, a legal deck can have no more than 4 of any non-basic-land.
    // Create a multiset to track the number of each kind of card
    std::multiset<std::string> dupes;
    // and the number of illegal cards. E.g., if we have 5 of one card, this will be set to 1. If we have 5 each of two different cards, will be set to 2.
    uint8_t dupe_count = 0;



    // SECOND: BASIC DYNAMICS!

    // Ideally, a deck will have 2 colors. 1 color is OK. 3 colors is maybe ok. >3 is just not going to work well. Let's count how many cards if each color we've seen.
    std::multiset<card::color> colors_seen;

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


    // NOW look at each card in turn.

    for (const auto &card : cards_)
    {

        // identify if this card is a dupe > 4
        dupes.insert(card.name);
        if (dupes.count(card.name) > 4)
        {
            ++dupe_count;
        }

        // update mana cost distribution
        cost_dist[card.converted_mana_cost]++;


        // update color distribution
        for (const auto &color : card.color_identity)
        {
            colors_seen.insert(color);
        }

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
        interaction_score /= cards_.size();
        interaction_scores.push_back(interaction_score); // if a card interacts with every other card in a deck, it will be worth 1 point, regardless of deck size
        reasons_["interaction_scores"][card.name] = interaction_score;

        ++i;
    }


    // Now that we've gone through each card, we can now think of the deck as a whole.

    rank_ = 0;

    // FIrst, let's make sure the deck size is OK. Notice that we remove duplicate references to the same physical card, so it is possible, even likely, that the deck size will be too small at first.
    rank_ -= 2 * (legal_deck_minimum - cards_.size());

    // Next, let's look at dupes, Each dupe is worth -1.
    rank_ -= dupe_count;
    reasons_["dupe_count"] = dupe_count;

    // Now let's look at the color distribution. The ideal distribution is an even number of two colors. Deviation from that will remove points
    // THe idea is that any card _not_ among the primary or secondary colors is worth -1
    // Any card in the primary in excess of whatever is in the secondary is worth -1
    card::color primary_color{card::color::colorless};
    card::color secondary_color{card::color::colorless};
    uint16_t primary_color_count{0};
    uint16_t secondary_color_count{0};
    uint16_t color_excess{0};
    for (const auto &color : card::all_colors)
    {
        // skip colorless cards, they don't count for this
        if (color == card::color::colorless)
        {
            continue;
        } //TODO what if we get a deck that is all one color + colorless? What then?
        uint16_t count = colors_seen.count(color);
        if (count > primary_color_count)
        {
            secondary_color_count = primary_color_count;
            secondary_color = primary_color;
            primary_color_count = count;
            primary_color = color;
        }
        else if (count > secondary_color_count)
        {
            secondary_color_count = count;
            secondary_color = color;
        }

    }
    // and now do it again to get the number of cards not in the primary or secondary colors
    for (const auto &color : card::all_colors)
    {
        // skip colorless cards, they don't count for this
        if (color == card::color::colorless) continue;

        if (color != primary_color && color != secondary_color)
        {
            color_excess += colors_seen.count(color);
        }
    }
    // finally, add the imbalance between primary and secondary
    color_excess += primary_color_count - secondary_color_count;

    rank_ -= 2 * color_excess;
    reasons_["color_primary"] = to_string(primary_color);
    reasons_["color_secondary"] = to_string(secondary_color);
    reasons_["colors_seen"] = colors_seen;


    // Now let's compute the distance from the ideal CMC distribution
    // TODO this should be something specified by the user
    std::array<double, 11> ideal_cmc_distribution{0.09, 0.09, 0.10, 0.18, 0.20, 0.18, 0.09, 0.05, 0.01, 0.01, 0.00};
    double cmc_distance{0.0};
    for (int i = cost_dist.size() - 1; i >= 0; --i)
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
            {card::type::creature,     0.600},
            {card::type::artifact,     0.0333},
            {card::type::enchantment,  0.10},
            {card::type::planeswalker, 0.0333},
            {card::type::instant,      0.13333},
            {card::type::sorcery,      0.10}
    };
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

    return rank_;
}

void to_json(nlohmann::json &j, const deck &d)
{
    j["rank"] = d.rank_;
    j["cards"] = d.cards_;
    j["reasons"] = d.reasons_;
}

void from_json(const nlohmann::json &j, deck &p)
{
//TODO
}


} //namespace magique