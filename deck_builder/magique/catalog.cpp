//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "catalog.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace magique
{
// TODO find a way to have case_insensitive std::unordered_map, instead of this ::tolower BS

catalog::catalog(std::string path)
    //catalog_filename, std::string annotations_filename)
{
    std::ifstream ifs(path + "/AllCards-x.json");
    nlohmann::json card_list_json;
    ifs >> card_list_json;
    ifs.close();

    ifs.open(path + "/card_tags.json");
    nlohmann::json mechanics;
    ifs >> mechanics;
    ifs.close();

    // AllJson from MTGJSON is one large object
    for (nlohmann::json::iterator card_kv = card_list_json.begin(); card_kv != card_list_json.end(); ++card_kv)
    {
        auto name = card_kv.key();

        auto card_json = card_kv.value();
        //load its annotations, if any
        try
        {
            card_json["mechanics"] = mechanics.at(name);
        }
        catch (nlohmann::json::out_of_range &e)
        {}

        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        cards_by_name_[name] = card_json.get<card>();
    }

}

const card &catalog::at(std::string name) const
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    card c;
    try
    {
        c = cards_by_name_.at(name);
    }
    catch (std::out_of_range e)
    {
        std::cerr << "Could not find a card named \"" << name  << "\"" << std::endl;
        throw (e);
    }

    return cards_by_name_.at(name);
}

}