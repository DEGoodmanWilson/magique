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
card::format deck::format{card::format::standard};
std::unordered_set<card::color> deck::color_identity{};
collection deck::collection{};
std::vector<card> deck::key_cards_{};
std::vector<evaluators::card_evaluator> deck::card_evaluators_{};

deck::deck(std::vector<uint64_t> indices) :
        rank_{0.0}
{
    // First, let's establish the color identity of this deck, if it hasn't been already.
    find_color_identity_(indices);

    // First, examine the cards in the indices, and select only certain of them to be considered, by inserting them into cards_



















    for (const auto &index : indices)
    {
        cards_.emplace_back(collection.at(index));
    }

    // Then, evaluate the deck as a whole

    // Next, evaluate the cards singly, and in pairs, and store the results
    std::unordered_set<std::string> card_reasons{};
    std::unordered_map<std::string, double> card_divisors{};
    std::unordered_map<std::string, double> card_evaluations{};

    reasons_["cards"] = nlohmann::json::object();
    for (const auto &card : cards_)
    {
        reasons_["cards"][card.name] = nlohmann::json::object();
        for (const auto &eval : card_evaluators_)
        {
            auto evaluation = eval(card, format);
            card_reasons.insert(evaluation.reason);
            if (card_divisors.count(evaluation.reason) == 0) card_divisors[evaluation.reason] = evaluation.scale;
            if (card_evaluations.count(evaluation.reason) == 0) card_evaluations[evaluation.reason] = 0.0;

            reasons_["cards"][card.name][evaluation.reason] = evaluation.score;
            card_evaluations[evaluation.reason] += evaluation.score;
        }
    }

    for (const auto &reason: card_reasons)
    {
        double card_score = card_evaluations[reason];
        double normalized_card_score = card_score / card_divisors[reason];
        reasons_[reason] = nlohmann::json::object();
        reasons_[reason]["score"] = card_score;
        reasons_[reason]["normalized_score"] = normalized_card_score;
        rank_ += normalized_card_score;
    }
}


void deck::find_color_identity_(std::vector<uint64_t> indices)
{
    std::unordered_set<card::color> top_colors{color_identity};
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

    //look at key cards. If there are key cards, add their color identity to the prefered color identity
    std::unordered_map<std::string, int64_t> indices_seen;
    for (const auto &card :key_cards_)
    {
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
    }

    auto size = top_colors.size();

    bool mandated_color_identity{(top_colors.size() > 0)};

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
    }

// identify the top N colors, if there is no mandate for a particular color id for the deck. Then use the top N colors as the deck identity

// TODO the bottleneck is here. I don't know why—this used to be plenty fast before!
    if (!mandated_color_identity)
    {
// identify the top N colors
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
//    j["cards"] = d.cards_;
    j["reasons"] = d.reasons_;
}

void from_json(const nlohmann::json &j, deck &p)
{
//TODO
}

} //namespace magique