//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "catalog.h"
#include <fstream>

namespace magique
{

catalog::catalog(std::string filename)
{
//    card foobar{
//            0,
//            "foobar",
//            {card::type::creature},
//            {card::color::red},
//            "Does damage",
//            4,
//            4,
//            1
//    };
//
//    cards_by_id_[foobar.id] = foobar;
//    cards_by_name_[foobar.name] = foobar;

    std::ifstream ifs(filename);
    nlohmann::json j{ifs};

    std::cout << j.type_name() << std::endl;

    //assume for now that it is just a vector of cards
    for(auto c_str: j)
    {
        card card;
        std::cout << c_str["name"].dump() << std::endl;
        from_json(c_str, card);
        cards_by_name_[card.name] = card;
    }
}

const card& catalog::at(std::string name)
{
    return cards_by_name_.at(name);
}

}