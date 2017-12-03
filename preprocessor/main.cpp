//
// Created by Don Goodman-Wilson on 24/11/2017.
//

#include <iostream>
#include <fstream>
#include <regex>
#include <json.hpp>


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
    nlohmann::json catalog;
    nlohmann::json collection{nlohmann::json::array()}; //initialize to an array

    {
        //open complete data file, and process it.
        std::ifstream ifs{"catalog.json"};
        ifs >> catalog;
    }

    {
        //open collection file, and process it
        std::ifstream ifs{"don_card_list.json"};
        nlohmann::json names;
        ifs >> names;

        for (const auto &item: names)
        {
            collection.push_back(catalog[item.get<std::string>()]);
        }
    }

    //Iterate over each card, and let's try to extract some features.
    for (auto &card: collection)
    {
        // Make sure that the card has some text. If it doesn't have any text, skip it.
        if (card["text"].is_null())
        {
            continue;
        }
        card["abilities"] = nlohmann::json::array();

        std::string text{card["text"].get<std::string>()};
        // strip rules explanations from text
        text = std::regex_replace(text, std::regex{R"( ?\(.+?\) ?)"}, "");

        // vigilance
        std::regex vigilance_regex{R"([Vv]igilance)"};
        std::smatch vigilance_smatch;
        if (is_creature(card) && std::regex_search(text, vigilance_smatch, vigilance_regex))
        {
            if (text.find("with vigilance") == std::string::npos && text.find("gain vigilance") == std::string::npos)
            {
                card["abilities"].emplace_back("vigilance");
            }
        }



        // flying
        std::regex flying_regex{R"([Ff]lying)"};
        std::smatch flying_smatch;
        if (is_creature(card) && std::regex_search(text, flying_smatch, flying_regex))
        {
            if (text.find("with flying") == std::string::npos && text.find("gain flying") == std::string::npos)
            {

                card["abilities"].emplace_back("flying");
            }
        }



        // trample
        std::regex trample_regex{R"([Tt]rample)"};
        std::smatch trample_smatch;
        if (is_creature(card) && std::regex_search(text, trample_smatch, trample_regex))
        {
            if (text.find("with trample") == std::string::npos && text.find("gain trample") == std::string::npos)
            {

                card["abilities"].emplace_back("trample");
            }
        }


        // reach
        std::regex reach_regex{R"([Rr]each)"};
        std::smatch reach_smatch;
        if (is_creature(card) && std::regex_search(text, reach_smatch, reach_regex))
        {
            if (text.find("with reach") == std::string::npos && text.find("gain reach") == std::string::npos)
            {

                card["abilities"].emplace_back("reach");
            }
        }


        // haste
        std::regex haste_regex{R"([Hh]aste)"};
        std::smatch haste_smatch;
        if (is_creature(card) && std::regex_search(text, haste_smatch, haste_regex))
        {
            if (text.find("with haste") == std::string::npos && text.find("gains haste") == std::string::npos)
            {

                card["abilities"].emplace_back("haste");
            }
        }

        // revolt
        std::regex revolt_regex{R"([Rr]evolt)"};
        std::smatch revolt_smatch;
        if (is_creature(card) && std::regex_search(text, revolt_smatch, revolt_regex))
        {
            card["abilities"].emplace_back("revolt");
        }


        // embalm
        std::regex embalm_regex{R"([Ee]mbalm)"};
        std::smatch embalm_smatch;
        if (is_creature(card) && std::regex_search(text, embalm_smatch, embalm_regex))
        {
            card["abilities"].emplace_back("embalm");
        }

        // token
        std::regex token_regex{R"([Tt]oken)"};
        std::smatch token_smatch;
        if (std::regex_search(text, token_smatch, token_regex))
        {
            card["abilities"].emplace_back("create_token");
        }

        // buff
        std::regex buff_regex{R"(\+[0-9]+\/\+[0-9]+)"};
        std::smatch buff_smatch;
        if (std::regex_search(text, buff_smatch, buff_regex))
        {
            card["abilities"].emplace_back("buff");
        }

        // debuff
        std::regex debuff_regex{R"(-[0-9]+\/-[0-9]+)"};
        std::smatch debuff_smatch;
        if (std::regex_search(text, debuff_smatch, debuff_regex))
        {
            card["abilities"].emplace_back("debuff");
        }


        // gain energy
        std::regex gain_energy_regex{R"([Gg]et \{E\})"};
        std::smatch gain_energy_smatch;
        if (std::regex_search(text, gain_energy_smatch, gain_energy_regex))
        {
            card["abilities"].emplace_back("energy_source");
        }

        // spend energy
        std::regex spend_energy_regex{R"([Pp]ay \{E\})"};
        std::smatch spend_energy_smatch;
        if (std::regex_search(text, spend_energy_smatch, spend_energy_regex))
        {
            card["abilities"].emplace_back("energy_sink");
        }


        // improvise
        std::regex improvise_regex{R"([Ii]mprovise)"};
        std::smatch improvise_smatch;
        if (std::regex_search(text, improvise_smatch, improvise_regex))
        {
            card["abilities"].emplace_back("improvise");
        }


        // fabricate
        std::regex fabricate_regex{R"([Ff]abricate)"};
        std::smatch fabricate_smatch;
        if (std::regex_search(text, fabricate_smatch, fabricate_regex))
        {
            card["abilities"].emplace_back("buff");
            card["abilities"].emplace_back("create_token");
        }


        // lifelink
        std::regex lifelink_regex{R"([Ll]ifelink)"};
        std::smatch lifelink_smatch;
        if (std::regex_search(text, lifelink_smatch, lifelink_regex))
        {
            card["abilities"].emplace_back("lifelink");
        }


        // deathtouch
        std::regex deathtouch_regex{R"([Dd]eathtouch)"};
        std::smatch deathtouch_smatch;
        if (std::regex_search(text, deathtouch_smatch, deathtouch_regex))
        {
            card["abilities"].emplace_back("deathtouch");
        }

        card["base_text"] = text;
    }
    std::cout << collection.dump(4) << std::endl;

}