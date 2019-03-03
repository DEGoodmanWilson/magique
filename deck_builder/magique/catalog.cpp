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
    std::ifstream ifs{path + "/AllCards.json"};
    nlohmann::json card_list_json;
    ifs >> card_list_json;
    ifs.close();

    ifs = std::ifstream{path + "/normalized_card_names.json"};
    nlohmann::json normalized_card_names_json;
    ifs >> normalized_card_names_json;
    ifs.close();

    // AllJson from MTGJSON is one large object
    for (const auto& card_kv : card_list_json.items())
    {
        auto name = card_kv.key();

        auto card_json = card_kv.value();

        // normalize the card name
        std::string normalized_card_name{normalized_card_names_json[name]};
        cards_by_name_[normalized_card_name] = card_json.get<card>();
    }

    std::cerr << "Loaded " << cards_by_name_.size() << " cards." << std::endl;
    std::cerr << "done." << std::endl;
}

card *catalog::at(const std::string &name)
{
    try
    {
        return &(cards_by_name_.at(name));
    }
    catch (std::out_of_range &e)
    {
        std::cerr << "Could not find a card named \"" << name << "\"" << std::endl;
        throw (e);
    }
}

}