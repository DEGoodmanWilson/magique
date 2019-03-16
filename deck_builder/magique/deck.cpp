//
// Created by Don Goodman-Wilson on 14/11/2017.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <shared_mutex>

#include "deck.h"

// TODO need a legality object to capture the context of legality, including deck size!

namespace magique
{

uint16_t deck::colors{2};
uint16_t deck::deck_minimum{60 - 26};
uint8_t deck::max_copies{4};
card::format deck::format{card::format::standard};
std::unordered_set<card::color> deck::color_identity{};
collection deck::collection{};
std::vector<card *> deck::key_cards_{};
std::vector<std::pair<evaluators::card_evaluator, double>> deck::card_evaluators_{};
std::vector<std::pair<evaluators::card_pair_evaluator, double>> deck::card_pair_evaluators_{};

std::unordered_map<std::string, std::vector<std::pair<evaluators::evaluation, double>>> card_evaluations_cache_;
std::shared_mutex card_evaluations_cache_mutex_;

std::unordered_map<std::string, std::vector<std::pair<evaluators::evaluation, double>>> card_pair_evaluations_cache_;
std::shared_mutex card_pair_evaluations_cache_mutex_;


deck::deck(std::vector<uint64_t> indices, bool calculate_reasons) :
        rank_{0.0}
{
    // First, let's cull the deck, and establish the color identity of this deck, if it hasn't been already.
    build_proposed_deck_(indices);

    std::unordered_set<std::string> card_reasons{};
    std::unordered_map<std::string, double> card_divisors{};
    std::unordered_map<std::string, double> card_evaluations{};

    // If we're storing reasons, initialize that too (most runs won't do this)
    if (calculate_reasons) reasons_["cards"] = nlohmann::json::object();

    // Next, evaluate the cards singly, and in pairs, and store the results
    for (const auto &kv : cards_)
    {
        const auto card_name = kv.first;
        const auto card = kv.second.second;
        const auto card_count = kv.second.first;
        if (calculate_reasons)
        {
            reasons_["cards"][card_name] = nlohmann::json::object();
            reasons_["cards"][card_name]["count"] = card_count;
        }

        // Let's evaluate this card by itself, first.

        std::vector<std::pair<evaluators::evaluation, double>> single_evaluations;
        // Have we already evaluated this card in another context? Pull it from the cache
        bool cached{false};
        {
            std::shared_lock<std::shared_mutex> l{card_evaluations_cache_mutex_};
            if (card_evaluations_cache_.count(card_name))
            {
                //pull from cache
                single_evaluations = card_evaluations_cache_.at(card_name);
                cached = true;
            }
        }
        if (!cached) // we'll need to do it the hard way
        {
            // iterate over all the evaluators, and evaluate!
            for (const auto &eval : card_evaluators_)
            {
                const auto evaluation{eval.first(card, card_count, format)};
                const auto weight{eval.second};
                single_evaluations.emplace_back(std::make_pair(evaluation, weight));

                // this shit we need to repeat in case it is cached. TODO DRY
                card_reasons.insert(evaluation.reason);
                if (card_divisors.count(evaluation.reason) == 0)
                {
                    card_divisors[evaluation.reason] = evaluation.scale / weight;
                }
                if (calculate_reasons)
                {
                    reasons_["cards"][card_name][evaluation.reason] = evaluation.score;
                }
                if (card_evaluations.count(evaluation.reason) == 0)
                {
                    card_evaluations[evaluation.reason] = 0.0;
                }
                card_evaluations[evaluation.reason] += evaluation.score * card_count;
                // DRY to here
            }
            {
                // write it to cache
                std::unique_lock<std::shared_mutex> l{card_evaluations_cache_mutex_};
                card_evaluations_cache_[card_name] = single_evaluations;
            }
        }
        else //we pulled it from cache
        {
            for (const auto &eval_weight : single_evaluations)
            {
                const auto evaluation{eval_weight.first};
                const auto weight{eval_weight.second};
                // this shit we need to repeat in case it is cached. TODO DRY
                card_reasons.insert(evaluation.reason);
                if (card_divisors.count(evaluation.reason) == 0)
                {
                    card_divisors[evaluation.reason] = evaluation.scale / weight;
                }
                if (calculate_reasons)
                {
                    reasons_["cards"][card_name][evaluation.reason] = evaluation.score;
                }
                if (card_evaluations.count(evaluation.reason) == 0)
                {
                    card_evaluations[evaluation.reason] = 0.0;
                }
                card_evaluations[evaluation.reason] += evaluation.score * card_count;
                // DRY to here
            }
        }


        // GREAT! That's the single-card evaluations. Now we need to compare this card against all the other cards.
        if (card_pair_evaluators_.size()) // do we even have pair evaluators? If not, skip this bit
        {
            for (const auto &kv2 : cards_)
            {
                const auto card_b_name = kv2.first;
                if (card_name == card_b_name) continue; // don't evaluate a card against itself
                const auto key{card_name + " + " + card_b_name};
                const auto card_b_count = kv.second.first;
                const auto card_b = kv2.second.second;
                if (calculate_reasons)
                {
                    reasons_["cards"][card_name][card_b_name] = nlohmann::json::object();
                    reasons_["cards"][card_name][card_b_name]["count"] = card_b_count;
                }

                std::vector<std::pair<evaluators::evaluation, double>> pair_evaluations;
                // Have we already evaluated this card in another context? Pull it from the cache
                bool cached{false};
                {
                    std::shared_lock<std::shared_mutex> l{card_pair_evaluations_cache_mutex_};
                    if (card_pair_evaluations_cache_.count(key))
                    {
                        //pull from cache
                        pair_evaluations = card_pair_evaluations_cache_.at(key);
                        cached = true;
                    }
                }
                if (!cached) // we'll need to do it the hard way
                {

                    // now, evaluate the pair the long way.
                    for (const auto &eval : card_pair_evaluators_)
                    {
                        const auto evaluation = eval.first(card, card_b, format);
                        const auto weight = eval.second;
                        pair_evaluations.emplace_back(std::make_pair(evaluation, weight));

                        // TODO DRY
                        card_reasons.insert(evaluation.reason);
                        if (card_divisors.count(evaluation.reason) == 0)
                        {
                            card_divisors[evaluation.reason] = evaluation.scale * deck_size_ / weight;
                        }
                        if (calculate_reasons)
                        {
                            reasons_["cards"][card_name][card_b_name][evaluation.reason] = evaluation.score;
                        }
                        if (card_evaluations.count(evaluation.reason) == 0)
                        {
                            card_evaluations[evaluation.reason] = 0.0;
                        }
                        card_evaluations[evaluation.reason] += evaluation.score * card_count * card_b_count;
                        // apply the evaluation to all copies of card_a and card_b
                        // TODO end DRY
                    }
                    {
                        // write it to cache
                        std::unique_lock<std::shared_mutex> l{card_pair_evaluations_cache_mutex_};
                        card_pair_evaluations_cache_[key] = pair_evaluations;
                    }
                }
                else // it was cached, we need to repeat some of the above
                {
                    for (const auto &eval_weight : pair_evaluations)
                    {
                        const auto evaluation{eval_weight.first};
                        const auto weight{eval_weight.second};

                        // TODO DRY
                        card_reasons.insert(evaluation.reason);
                        if (card_divisors.count(evaluation.reason) == 0)
                        {
                            card_divisors[evaluation.reason] = evaluation.scale * deck_size_ / weight;
                        }
                        if (calculate_reasons)
                        {
                            reasons_["cards"][card_name][card_b_name][evaluation.reason] = evaluation.score;
                        }
                        if (card_evaluations.count(evaluation.reason) == 0)
                        {
                            card_evaluations[evaluation.reason] = 0.0;
                        }
                        card_evaluations[evaluation.reason] += evaluation.score * card_count * card_b_count;
                        // apply the evaluation to all copies of card_a and card_b
                        // TODO end DRY
                    }
                }
            }
        }

        for (const auto &reason: card_reasons) // TODO can't we do this without another loop? Maybe not because the division…
        {
            double card_score = card_evaluations[reason];
            double normalized_card_score = card_score / card_divisors[reason];
            if (calculate_reasons)
            {
                //TODO FIX THIS
                reasons_[reason] = nlohmann::json::object();
                reasons_[reason]["score"] += card_score;
                reasons_[reason]["normalized_score"] += normalized_card_score;
                reasons_[reason]["divisor"] += card_divisors[reason];
            }
            rank_ += normalized_card_score;
            if (rank_ < 0)
            {
                std::cerr << reasons_.dump() << std::endl;
                std::cerr << "Negative rank!" << std::endl;
                rank_ = 0.0;
            }
        }
    }
}


void deck::build_proposed_deck_(std::vector<uint64_t> indices)
{
    std::unordered_set<card::color> prefered_color_identity{color_identity};
    // remove any elements that appear twice
    std::unordered_multiset<uint64_t> collection_duplicates;
    // observe the colors that we have seen in the proposed deck
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
        if (indices_seen.count(card->name)) last_index = indices_seen[card->name];
        auto index = collection.index_at(card->name, last_index);
        indices_seen[card->name] = index;
        collection_duplicates.insert(index); // TODO handle multiple key cards of same name!

        if (cards_.count(card->name))
        { //if we have seen this card already
            if (cards_.at(card->name).first < max_copies)
            { // but only insert it if we have fewer than 4 in the deck
                cards_[card->name].first++;
            }
            else
            {
                cards_[card->name] = std::make_pair(1, card);
            }
        }

        for (const auto &color : card->color_identity)
        {
            prefered_color_identity.insert(color);
        }
    }

    auto size = prefered_color_identity.size();

    bool mandated_color_identity{(prefered_color_identity.size() > 0)};

    for (const auto &i : indices)
    {
        if (collection_duplicates.count(i) >= 1) continue; // skip dupe references to same physical card

        collection_duplicates.insert(i);

        // what card does this index represent?
        auto card = collection.at(i);

        // insert request card into deck
        // TODO manage restricted list, and legality too
        if (cards_.count(card->name))
        { //if we have seen this card already
            cards_[card->name].first++;

            if (cards_[card->name].first > max_copies)
            {
                cards_[card->name].first = max_copies;
            }
        }
        else
        {
            // TODO handle restricted cards
            if (card->legalities[format] == card::legality::legal)
            { //only add if legal in this format.
                cards_[card->name] = std::make_pair(1, card);
            }
        }

        // handle color identity
        if (!mandated_color_identity)
        {
            for (const auto &color : card->color_identity)
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
            prefered_color_identity.insert(std::get<card::color>(sorted_colors[i]));
        }
    }

    // now remove all cards not of the final color identity
    // TODO can this be improved? ALl this color stuff feels pretty clunky.
    std::vector<std::string> to_remove;
    for (auto &kv : cards_)
    {
        for (const auto card_color : kv.second.second->color_identity)
        {
            if (card_color == card::color::colorless) continue;

            if (prefered_color_identity.count(card_color) == 0)
            {
                to_remove.push_back(kv.first);
                break;
            }
        }
    }
    for (const auto &name : to_remove)
    {
        cards_.erase(name);
    }

    // Finally, add the keycards back in
    for (auto key_card : key_cards_)
    {
        cards_.emplace(key_card->name, std::pair<uint16_t, card *>(1, key_card));
    }

    deck_size_ = 0;
    for (const auto &kv : cards_)
    {
        deck_size_ += kv.second.first;
    }

    // TODO calculate mana curve
    // TODO calculate card type distribution
}

void to_json(nlohmann::json &j, const deck &d)
{
    j = nlohmann::json{{"_list",  nlohmann::json::array()},
                       {"_count", 0}};
    uint16_t deck_size{0};
    for (const auto &kv: d.cards_)
    {
        const auto card_name = kv.first;
        const auto count = kv.second.first;
        for (uint16_t i = 0; i < count; ++i)
        {
            j["_list"].push_back(card_name);
        }
        deck_size += count;
    }
    std::sort(j["_list"].begin(), j["_list"].end());
    j["_count"] = deck_size;

    j["rank"] = d.rank_;
    j["reasons"] = d.reasons_;
}

void from_json(const nlohmann::json &j, deck &p)
{
//TODO
}

} //namespace magique