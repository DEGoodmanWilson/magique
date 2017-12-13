//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "catalog.h"
#include <json.hpp>
#include <fstream>

namespace magique
{

catalog::catalog(std::string filename, std::string annotations_filename)
{
    std::ifstream ifs(filename);
    nlohmann::json card_list_json{ifs};
    ifs.close();

    nlohmann::json annotations;
    if(!annotations_filename.empty())
    {
        ifs.open(annotations_filename);
        annotations << ifs;
    }


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
    }

    cards_by_name_ = card_list_json.get<decltype(cards_by_name_)>();
}

const card& catalog::at(std::string name) const
{
    card c;
    try
    {
        c = cards_by_name_.at(name);
    }
    catch(std::out_of_range e)
    {
        std::cout << name << std::endl;
        throw(e);
    }

    return cards_by_name_.at(name);
}

}