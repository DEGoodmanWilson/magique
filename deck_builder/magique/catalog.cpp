//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "catalog.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace magique
{

catalog::catalog(std::string catalog_filename, std::string annotations_filename)
{
    std::ifstream ifs(catalog_filename);
    nlohmann::json card_list_json;
    ifs >> card_list_json;
    ifs.close();

    ifs.open(annotations_filename);
    nlohmann::json annotations;
    ifs >> annotations;
    ifs.close();


    /* annotations look like:
     * {
     *   "card_name": {
     *     "key": value,
     *     "key2": {
     *       "key2valuekey": value
     *     }
     *   }
     * }
     */



    // AllJson from MTGJSON is one large object
    for (nlohmann::json::iterator card_kv = card_list_json.begin(); card_kv != card_list_json.end(); ++card_kv)
    {
        auto name = card_kv.key();
        auto card_json = card_kv.value();
        //load its annotations, if any
        try
        {
            const auto a = annotations[name];
            for (auto it = a.begin(); it != a.end(); ++it)
            {
                card_json[it.key()] = it.value();
            }
        }
        catch (std::out_of_range e)
        {}
        cards_by_name_[name] = card_json.get<card>();
    }
    //        cards_by_name_ = card_list_json.get<decltype(cards_by_name_)>();

}

const card &catalog::at(std::string name) const
{
    card c;
    try
    {
        c = cards_by_name_.at(name);
    }
    catch (std::out_of_range e)
    {
        std::cout << name << std::endl;
        throw (e);
    }

    return cards_by_name_.at(name);
}

}