//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "catalog.h"
#include <json.hpp>
#include <fstream>

namespace magique
{

bool case_insensitive_comp_::operator()(const std::string &a, const std::string &b) const noexcept
{
    return strcasecmp(a.c_str(), b.c_str()) < 0;
}


catalog::catalog(std::string filename, std::string annotations_filename)
{
    std::ifstream ifs(filename);
    nlohmann::json card_list_json{ifs};
    ifs.close();

    ifs.open(annotations_filename);
    nlohmann::json annotations{ifs};


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



    //assume for now that it is just a vector of cards
    for(auto card_json: card_list_json)
    {
        auto name = card_json["name"].get<std::string>();
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

}

const card& catalog::at(std::string name) const
{
    return cards_by_name_.at(name);
}

}