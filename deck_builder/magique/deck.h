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
#include "interactions.h"
#include "evaluators/types.h"

namespace magique
{

class deck;

void to_json(nlohmann::json &j, const deck &p);

void from_json(const nlohmann::json &j, deck &p);

class deck
{
public:
    deck(std::vector<uint64_t> indices);

    static void add_key_card(card key)
    {
        key.bonus_multiplier = 2.0;
        key_cards_.push_back(key);
    }

    double evaluate() { return rank_; }

    friend void to_json(nlohmann::json &j, const deck &p);

    friend void from_json(const nlohmann::json &j, deck &p);

    static uint16_t colors;
    static std::unordered_set<card::color> color_identity;
    static collection collection;

    static uint16_t deck_minimum;

    static card::format format;

    static void add_evaluator(evaluators::card_evaluator eval_func)
    {
        card_evaluators_.emplace_back(eval_func);
    }

private:
    static std::vector<evaluators::card_evaluator> card_evaluators_;
    static std::vector<card> key_cards_;

    std::vector<card> cards_;
    double rank_;
    nlohmann::json reasons_;

    void find_color_identity_(std::vector<uint64_t> indices);
};


} //namespace magique
