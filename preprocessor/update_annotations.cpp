//
// Created by Don Goodman-Wilson on 24/11/2017.
//

#include <iostream>
#include <fstream>
#include <regex>
#include <json.hpp>
#include "../magique/card.h"
#include "../magique/collection.h"

int main(void)
{
    // fire up a catalog
    std::string catalog_filename{"data/catalog.json"};
    std::string annotations_filename{"data/annotations.json"};
    std::string collection_filename{"data/collection_dons.csv"};

    magique::catalog catalog{catalog_filename, annotations_filename};
    magique::collection collection{collection_filename, catalog};

    nlohmann::json output = nlohmann::json::object();

    for(auto i = 0; i < collection.count(); ++i)
    {
        auto card = collection.at(i);
        output[card.name] = {
                {"abilities", card.abilities},
                {"affinities", card.affinities},
                {"triggers", card.triggers}
        };

        if(card.abilities.empty() && card.affinities.empty() && card.triggers.empty() && !card.text.empty())
        {
            output[card.name]["text"] = card.text;
        }
    }

    std::cout << std::setw(2) << output << std::endl;

}