//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#pragma once

#include <string>
#include <map>
#include "card.h"

namespace magique
{


struct case_insensitive_comp_ {
    bool operator()(const std::string& a, const std::string& b) const noexcept;
};

class catalog
{
public:
    catalog(std::string filename, std::string annotations_filename="");

    const card &at(std::string name) const;

private:
    std::map<std::string, card, case_insensitive_comp_> cards_by_name_;
};

}