//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#pragma once

#include <string>
#include <map>
#include "card.h"

namespace magique
{

class catalog
{
public:
    catalog(std::string filename);

    const card &at(std::string name);

private:
    std::map<card::id_t, card> cards_by_id_;
    std::map<std::string, card> cards_by_name_;
};

}