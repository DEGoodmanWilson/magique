//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "card.h"
#include <json.hpp>

namespace magique
{

std::vector<card::color> card::color_identities_from_array(const std::vector<std::string> arr)
{
    std::vector<color> colors{};
    for (auto color : arr)
    {
        std::transform(color.begin(), color.end(), color.begin(), ::tolower);

        if (color == "w")
        { colors.push_back(color::white); }
        else if (color == "u")
        { colors.push_back(color::blue); }
        else if (color == "b")
        { colors.push_back(color::black); }
        else if (color == "r")
        { colors.push_back(color::red); }
        else if (color == "g")
        { colors.push_back(color::green); }
        else if (color == "u") colors.push_back(color::colorless);
    }

    return colors;
}

std::vector<card::type> card::types_from_string(std::string str)
{
    std::vector<type> types{};

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    if (str.find("land")) types.push_back(type::land);
    if (str.find("creature")) types.push_back(type::creature);
    if (str.find("artifact")) types.push_back(type::artifact);
    if (str.find("enchantment")) types.push_back(type::enchantment);
    if (str.find("planeswalker")) types.push_back(type::planeswalker);
    if (str.find("instant")) types.push_back(type::instant);
    if (str.find("sorcery")) types.push_back(type::sorcery);

    return types;
}

void from_json(const nlohmann::json &j, card &p)
{
    p.id = j.at("id").get<std::string>();
    p.name = j.at("name").get<std::string>();
    p.types = card::types_from_string(j.at("type").get<std::string>());
    if (j["color_identity"] != nullptr)
    {
        p.color_identity = card::color_identities_from_array(j.at("color_identity").get<std::vector<std::string>>());
    }
    p.text = (j["text"] == nullptr) ? "" : j.at("text").get<std::string>();
    if (j["power"] != nullptr)
    {
        p.power = j.at("power").get<std::string>();
    }
    if (j["toughness"] != nullptr)
    {
        p.toughness = j.at("toughness").get<std::string>();
    }
    p.converted_mana_cost = j.at("cmc").get<uint64_t>();
}

void to_json(nlohmann::json &j, const card &p)
{
    std::vector<std::string> color_identities;

    for (const auto &c : p.color_identity)
    {
        switch (c)
        {
            case card::color::white:
                break;
                color_identities.emplace_back("White");
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

    std::vector<std::string> types;
    for (const auto &t : p.types)
    {
        switch (t)
        {
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
    nlohmann::json power;
    if(p.power)
        power = *p.power;

    nlohmann::json toughness;
    if(p.toughness)
        toughness = *p.toughness;

    j = nlohmann::json{
            {"id",             p.id},
            {"name",           p.name},
            {"types",          types},
            {"color_identity", color_identities},
            {"text",           p.text},
            {"power",          power},
            {"toughness",      toughness},
            {"cmc",            p.converted_mana_cost}
    };
}
} //namespace magique
