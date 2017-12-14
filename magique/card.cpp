//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include <regex>
#include "card.h"
#include <json.hpp>

namespace magique
{


std::unordered_map<std::string, card::type> card::strings_for_types{
        {"Basic Land",   card::type::basic_land},
        {"Land",         card::type::land},
        {"Creature",     card::type::creature},
        {"Artifact",     card::type::artifact},
        {"Enchantment",  card::type::enchantment},
        {"Planeswalker", card::type::planeswalker},
        {"Instant",      card::type::instant},
        {"Sorcery",      card::type::sorcery},
};

std::set<card::color> card::color_identities_from_array(const std::set<std::string> arr)
{
    std::set<color> colors{};
    for (auto color : arr)
    {
        std::transform(color.begin(), color.end(), color.begin(), ::tolower);

        if (color == "w")
        { colors.emplace(color::white); }
        else if (color == "u")
        { colors.emplace(color::blue); }
        else if (color == "b")
        { colors.emplace(color::black); }
        else if (color == "r")
        { colors.emplace(color::red); }
        else if (color == "g")
        { colors.emplace(color::green); }
        else if (color == "u") colors.emplace(color::colorless);
    }

    return colors;
}

bool card::has_type(const std::string &type_str) const
{
    if(strings_for_types.count(type_str) == 0) return false;
    auto t = strings_for_types.at(type_str);

    if((t==type::land) && types.count(type::basic_land)) return true; // basic lands are also lands

    return types.count(t) > 0;
}

std::set<card::type>
card::types_from_array(const std::set<std::string> &types, const std::set<std::string> &supertypes)
{
    std::set<type> types_arr{};

    for (const auto &type : types)
    {
        std::string type_str = type;
        std::transform(type_str.begin(), type_str.end(), type_str.begin(), ::tolower);

        if (type_str == "land")
        {
            bool basic_land{false};
            for (const auto &supertype : supertypes)
            {
                std::string supertype_str = supertype;
                std::transform(supertype_str.begin(), supertype_str.end(), supertype_str.begin(), ::tolower);

                if (supertype_str == "basic")
                {
                    basic_land = true;
                    break;
                }
            }
            if (basic_land)
            {
                types_arr.emplace(type::basic_land);
            }

            types_arr.emplace(type::land); // basic lands are also lands
        }
        else if (type_str == "creature")
        { types_arr.emplace(type::creature); }
        else if (type_str == "artifact")
        { types_arr.emplace(type::artifact); }
        else if (type_str == "enchantment")
        { types_arr.emplace(type::enchantment); }
        else if (type_str == "planeswalker")
        { types_arr.emplace(type::planeswalker); }
        else if (type_str == "instant")
        { types_arr.emplace(type::instant); }
        else if (type_str == "sorcery") types_arr.emplace(type::sorcery);

    }


    return types_arr;
}

void from_json(const nlohmann::json &j, card &p)
{
    p.name = j.at("name").get<std::string>();

    std::set<std::string> supertypes;
    try
    {
        if (j.at("supertypes").is_array())
        {
            supertypes = j.at("supertypes").get<std::set<std::string>>();
        }
    }
    catch (std::out_of_range e)
    {}

    try
    {
        p.types = card::types_from_array(j.at("types").get<std::set<std::string>>(), supertypes);
    }
    catch (std::out_of_range e)
    {}

    try
    {
        if (j.at("subtypes").is_array())
        {
            p.subtypes = j.at("subtypes").get<std::set<std::string>>();
        }
    }
    catch (std::out_of_range e)
    {}


    try
    {
        p.color_identity = card::color_identities_from_array(j.at("colorIdentity").get<std::set<std::string>>());
    }
    catch (std::out_of_range e)
    {}

    try
    {
        p.text = j.at("text").get<std::string>();
    }
    catch (std::out_of_range e)
    {}


    try
    {
        p.power = j.at("power").get<std::string>();
    }
    catch (std::out_of_range e)
    {}


    try
    {
        p.toughness = j.at("toughness").get<std::string>();
    }
    catch (std::out_of_range e)
    {}

    p.converted_mana_cost = j.at("cmc").get<uint64_t>();

    try
    {
        for (const auto &a : j.at("abilities").get<std::vector<std::string>>())
        {
            p.abilities.emplace(a);
        }
    }
    catch (std::out_of_range e)
    {}

    try
    {
        for (const auto &a : j.at("affinities").get<std::vector<std::string>>())
        {
            p.affinities.emplace(a);
        }
    }
    catch (std::out_of_range e)
    {}

}

std::string to_string(card::type t)
{
    switch (t)
    {
        case card::type::basic_land:
            return "basic land";
        case card::type::land:
            return "land";
        case card::type::creature:
            return "creature";
        case card::type::artifact:
            return "artifact";
        case card::type::enchantment:
            return "enchantment";
        case card::type::planeswalker:
            return "planeswalker";
        case card::type::instant:
            return "instant";
        case card::type::sorcery:
            return "sorcery";
    }
}

void to_json(nlohmann::json &j, const card &p)
{
    j = nlohmann::json::object();
    std::vector<std::string> color_identities;
    for (const auto &c : p.color_identity)
    {
        switch (c)
        {
            case card::color::white:
                color_identities.emplace_back("White");
                break;
            case card::color::blue:
                color_identities.emplace_back("Blue");
                break;
            case card::color::black:
                color_identities.emplace_back("Black");
                break;
            case card::color::red:
                color_identities.emplace_back("Red");
                break;
            case card::color::green:
                color_identities.emplace_back("Green");
                break;
            case card::color::colorless:
                color_identities.emplace_back("Colorless");
                break;
        }
    }
    if(color_identities.size())
    {
        j["color_identities"] = color_identities;
    }

    std::vector<std::string> types;
    for (const auto &t : p.types)
    {
        switch (t)
        {
            case card::type::basic_land:
                types.emplace_back("Basic Land");
                break;
            case card::type::land:
                types.emplace_back("Land");
                break;
            case card::type::creature:
                types.emplace_back("Creature");
                break;
            case card::type::artifact:
                types.emplace_back("Artifact");
                break;
            case card::type::enchantment:
                types.emplace_back("Enchantment");
                break;
            case card::type::planeswalker:
                types.emplace_back("Planeswalker");
                break;
            case card::type::instant:
                types.emplace_back("Instant");
                break;
            case card::type::sorcery:
                types.emplace_back("Sorcery");
                break;
        }
    }
    if(types.size())
    {
        j["types"] = types;
    }

    if (p.subtypes.size() > 0)
    {
        j["subtypes"] = p.subtypes;
    }

    if (p.power)
    {
        j["power"] = *p.power;
    }

    if (p.toughness)
    {
        j["toughness"] = *p.toughness;
    }

    if (p.abilities.size())
    {
        j["abilities"] = p.abilities;
    }
    if (p.affinities.size())
    {
        j["affinities"] = p.affinities;
    }

    j["name"] = p.name;

    if(!p.text.empty())
    {
        j["text"] = p.text;
    }

    j["cmc"] = p.converted_mana_cost;

    //TODO actual mana cost!
}
} //namespace magique
