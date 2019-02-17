//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#pragma once

#include <string>
#include <vector>
#include <locale>
#include <set>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <experimental/optional>

namespace magique
{

/***
{
  "name": "Aerial Responder",
  "layout": "normal",
  "names": null,
  "manaCost": "{1}{W}{W}",
  "convertedManaCost": 3,
  "colors": [
    "White"
  ],
  "colorIdentity": [
    "W"
  ],
  "type": "Creature \u2014 Dwarf Soldier",
  "supertypes": null,
  "subtypes": [
    "Dwarf",
    "Soldier"
  ],
  "rarity": "Uncommon",
  "text": "Flying, vigilance, lifelink",
  "flavor": "Dwarves have an instinct for repair, an inherent understanding of how systems work, and a reputation for fearlessness. They're perfect candidates for the Fair's emergency response team.",
  "artist": "Raoul Vitale",
  "number": "2",
  "power": "2",
  "toughness": "3",
  "loyalty": null,
  "variations": null,
  "watermark": null,
  "border": null,
  "timeshifted": null,
  "hand": null,
  "life": null,
  "release_date": null,
  "starter": null,
  "rulings": null,
  "foreign_names": null,
  "printings": [
    "KLD",
    "E01"
  ],
  "original_text": "Flying, vigilance, lifelink",
  "original_type": "Creature \u2014 Dwarf Soldier",
  "legalities": [
    {
      "format": "Commander",
      "legality": "Legal"
    },
    {
      "format": "Kaladesh Block",
      "legality": "Legal"
    },
    {
      "format": "Legacy",
      "legality": "Legal"
    },
    {
      "format": "Modern",
      "legality": "Legal"
    },
    {
      "format": "Standard",
      "legality": "Legal"
    },
    {
      "format": "Vintage",
      "legality": "Legal"
    }
  ],
  "source": null,
  "image_url": "http:\/\/gatherer.wizards.com\/Handlers\/Image.ashx?multiverseid=430541&type=card",
  "set": "E01",
  "set_name": "Archenemy: Nicol Bolas",
  "id": "d43016981b23288ff8faf3dfac674cdd1560a719",

  # Added by mechanics discovery
  "mechanics": [1, 2, 3]
}
*/


struct card
{
    using id_t = uint64_t;
    enum class color
    {
        white,
        blue,
        black,
        red,
        green,
        colorless,
    };
    static constexpr std::array<color, 6> all_colors = {color::white, color::blue, color::black, color::red,
                                                        color::green, color::colorless};

    enum class type
    {
        basic_land,
        land,
        creature,
        artifact,
        enchantment,
        planeswalker,
        instant,
        sorcery,
    };
    static constexpr std::array<type, 8> all_types = {type::basic_land, type::land, type::creature, type::artifact,
                                                      type::enchantment, type::planeswalker, type::instant,
                                                      type::sorcery};


    // TODO format is a feature of a deck, not a card, really. kinda.
    // TODO using an enum like this prevents "block" formats
    enum class format
    {
        commander,
        legacy,
        modern,
        pauper,
        standard,
        vintage,
        onevone,
        duel,
        penny,
        frontier,
        brawl,
        other
    };

    enum class legality
    {
        legal,
        banned,
        restricted
    };

    static std::unordered_map<std::string, type> strings_for_types;

    bool has_type(const std::string &type_str) const;

    static std::set<color> color_identities_from_array(std::set<std::string> arr);

    static std::set<card::type>
    types_from_array(const std::set<std::string> &types, const std::set<std::string> &supertypes);

//    std::string id;
    std::string name;
    std::set<type> types;
    std::set<std::string> subtypes;
    std::set<color> color_identity;

    // TODO this doesn't handle the restricted list!
    std::unordered_map<format, legality> legalities{
            {card::format::commander, card::legality::legal},
            {card::format::legacy,    card::legality::legal},
            {card::format::modern,    card::legality::legal},
            {card::format::pauper,    card::legality::legal},
            {card::format::standard,  card::legality::legal},
            {card::format::vintage,   card::legality::legal},
            {card::format::onevone,   card::legality::legal},
            {card::format::duel,      card::legality::legal},
            {card::format::penny,     card::legality::legal},
            {card::format::frontier,     card::legality::legal},
            {card::format::brawl,     card::legality::legal},
            {card::format::other,     card::legality::legal}
    };
    std::string text;
    std::experimental::optional<int8_t> power;
    std::experimental::optional<int8_t> toughness;
    uint8_t converted_mana_cost{0};

    std::vector<uint64_t> mechanics;

    double bonus_multiplier;
};


std::string to_string(const card::type &t);

std::string to_string(const card::color &c);


void to_json(nlohmann::json &j, const card::color &c);

void to_json(nlohmann::json &j, const card::type &t);

void to_json(nlohmann::json &j, const card &p);

void from_json(const nlohmann::json &j, card &p);

} // namespace magique
