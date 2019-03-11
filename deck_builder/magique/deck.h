//
// Created by Don Goodman-Wilson on 14/11/2017.
//

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include "card.h"
#include "catalog.h"
#include "collection.h"
#include "interactions.old.h"
#include "evaluators/types.h"

namespace magique
{

class deck;

void to_json(nlohmann::json &j, const deck &p);

void from_json(const nlohmann::json &j, deck &p);

class deck
{
public:
    deck(std::vector<uint64_t> indices, bool calculate_reasons=false);

    static void add_key_card(card *key)
    {
        key->bonus_multiplier = 2.0;
        key_cards_.push_back(key);
    }

    double evaluate()
    { return rank_; }

    friend void to_json(nlohmann::json &j, const deck &p);

    friend void from_json(const nlohmann::json &j, deck &p);

    static uint16_t colors;
    static std::unordered_set<card::color> color_identity;
    static collection collection;

    static uint16_t deck_minimum;
    static uint8_t max_copies;

    static card::format format;

    static void add_evaluator(evaluators::card_evaluator eval_func, double weight)
    {
        card_evaluators_.emplace_back(std::make_pair(eval_func, weight));
    }

    static void add_evaluator(evaluators::card_pair_evaluator eval_func, double weight)
    {
        card_pair_evaluators_.emplace_back(std::make_pair(eval_func, weight));
    }

    static void add_evaluator(evaluators::deck_evaluator eval_func, double weight)
    {
        deck_evaluators_.emplace_back(std::make_pair(eval_func, weight));
    }

private:
    static std::vector<std::pair<evaluators::card_evaluator, double>> card_evaluators_;
    static std::vector<std::pair<evaluators::card_pair_evaluator, double>> card_pair_evaluators_;
    static std::vector<std::pair<evaluators::deck_evaluator, double>> deck_evaluators_;
    static std::vector<card *> key_cards_;

    //TODO use set or multiset, and count the copies of each card.
    std::unordered_map<std::string, std::pair<uint16_t, card *>> cards_;
    uint16_t deck_size_;
    double rank_;
    nlohmann::json reasons_;

    void build_proposed_deck_(std::vector<uint64_t> indices);
};


} //namespace magique
