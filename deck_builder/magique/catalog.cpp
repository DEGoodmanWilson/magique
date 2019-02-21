//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "catalog.h"
#include "../TextUtils/TextUtils.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace magique
{
// TODO find a way to have case_insensitive std::unordered_map, instead of this ::tolower BS

catalog::catalog(std::string path)
//catalog_filename, std::string annotations_filename)
{
    std::cerr << "Loading card data...";
    std::ifstream ifs(path + "/AllCards.json");
    nlohmann::json card_list_json;
    ifs >> card_list_json;
    ifs.close();
    std::cerr << "done." << std::endl;

    std::cerr << "Loading tag data...";
    ifs.open(path + "/card_tags.json");
    nlohmann::json mechanics;
    ifs >> mechanics;
    ifs.close();
    std::cerr << "done." << std::endl;

    // AllJson from MTGJSON is one large object
    for (nlohmann::json::iterator card_kv = card_list_json.begin(); card_kv != card_list_json.end(); ++card_kv)
    {
        auto name = card_kv.key();

        auto card_json = card_kv.value();

        // ignore meld cards
        if (card_json["layout"] == "meld")
        {
            continue;
        }

        // see if it is a split or transform or other kind multi-card. If so , use it's canonical name by joining the names in the "names" field with " // "
        if (card_json.find("names") != card_json.end())
        {
            const auto names = card_json["names"].get<std::vector<std::string>>();
            if(names.size() > 1)
            {
                name = TextUtils::Join(card_json["names"].get<std::vector<std::string>>(), " // ");
                // make sure we haven't already included it
                if (cards_by_name_.count(name))
                {
                    continue;
                }
            }
        }

        std::transform(name.begin(), name.end(), name.begin(), ::tolower);

        //load its annotations, if any
        try
        {
            card_json["mechanics"] = mechanics.at(name);
        }
        catch (nlohmann::json::out_of_range &e)
        {}

        cards_by_name_[name] = card_json.get<card>();
    }

}

card *catalog::at(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    try
    {
        return &(cards_by_name_.at(name));
    }
    catch (std::out_of_range &e)
    {
        std::cerr << "Could not find a card named \"" << name << "\"" << std::endl;
        throw (e);
    }

    return nullptr; // should never get here.
}

}