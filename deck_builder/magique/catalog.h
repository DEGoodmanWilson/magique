//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include "card.h"

namespace magique
{

class catalog
{
public:
    catalog() = default;

    catalog(std::string path);

    card *at(std::string name);

    std::unordered_map<std::string, card>::iterator begin() noexcept
    {
        return cards_by_name_.begin();
    }

    std::unordered_map<std::string, card>::iterator end() noexcept
    {
        return cards_by_name_.end();
    }

    std::unordered_map<std::string, card>::const_iterator begin() const noexcept
    {
        return cards_by_name_.cbegin();
    }

    std::unordered_map<std::string, card>::const_iterator end() const noexcept
    {
        return cards_by_name_.cend();
    }

private:
    std::unordered_map<std::string, card> cards_by_name_; // owner of all cards. Will outlive everything else.
};

}