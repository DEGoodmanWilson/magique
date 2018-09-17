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

private:
    std::unordered_map<std::string, card> cards_by_name_; // owner of all cards. Will outlive everything else.
};

}