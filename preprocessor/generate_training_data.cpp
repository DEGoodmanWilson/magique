//
// Created by Don Goodman-Wilson on 24/11/2017.
//

#include <iostream>
#include <fstream>
#include <regex>
#include <nlohmann/json.hpp>
#include "../magique/card.h"


bool is_creature(const nlohmann::json &card)
{
    for (const auto &type : card["types"])
    {
        if (type.get<std::string>() == "Creature")
        {
            return true;
        }
    }

    return false;
}


int main(void)
{
    // fire up a catalog
    std::string filename{"tests/data/catalog_limited.json"};
    std::string annotations_filename{"data/annotations.json"};

    std::ifstream ifs(filename);
    nlohmann::json card_list_json{ifs};
    ifs.close();

    nlohmann::json annotations;
    if (!annotations_filename.empty())
    {
        ifs.open(annotations_filename);
        annotations << ifs;
    }

    nlohmann::json card_label_array{nlohmann::json::array()};
    nlohmann::json card_text_array{nlohmann::json::array()};
    nlohmann::json card_class_array{nlohmann::json::array()};
    nlohmann::json output{nlohmann::json::object()};


    //assume for now that it is just a vector of cards
    for (auto &card_json: card_list_json)
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




        magique::card card = card_json;


        std::string text{card.text};
        // strip rules explanations from text
        text = std::regex_replace(text, std::regex{R"( ?\(.+?\) ?)"}, "");
        text = std::regex_replace(text, std::regex{" ?"+name}, "");

        if(card_json["abilities"].is_null())
        {
            card_json["abilities"] = nlohmann::json::array();
        }


        card_label_array.insert(card_label_array.end(), name);
        card_text_array.insert(card_text_array.end(), text);
        card_class_array.insert(card_class_array.end(), card.abilities);

    }

    output["labels"] = card_label_array;
    output["text"] = card_text_array;
    output["classes"] = card_class_array;

    std::cout << std::setw(2) << output << std::endl;

}