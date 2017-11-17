//
// Created by Don Goodman-Wilson on 14/11/2017.
//

#pragma once

#include <string>
#include <vector>
#include <map>
#include <json.hpp>
#include "card.h"
#include "catalog.h"
#include "collection.h"

namespace magique
{

class deck;

void to_json(nlohmann::json &j, const deck &p);

void from_json(const nlohmann::json &j, deck &p);

class deck
{
public:
    deck(const std::string &filename, const catalog &catalog);

    deck(const std::vector<uint64_t> &indices, const collection &collection);

    static void add_key_card(card key)
    {
        key_cards_.push_back(key);
    }


    double eval();

    friend void to_json(nlohmann::json &j, const deck &p);

    friend void from_json(const nlohmann::json &j, deck &p);

public:
    std::vector<card> cards_;
    double rank_;
    std::array<uint8_t, 11> cost_dist_;
    std::map<card::type, uint8_t> type_dist_;
    bool legal_;
    uint8_t colors_;
    nlohmann::json reasons_;
    static std::vector<card> key_cards_;
};


} //namespace magique
