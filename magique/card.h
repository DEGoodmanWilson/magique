//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#pragma once

#include <string>
#include <vector>
#include <locale>
#include <set>
#include <unordered_map>
#include <json.hpp>
#include <experimental/optional>

namespace magique
{

/***
{
  "name": "Aerial Responder",
  "multiverse_id": 430541,
  "layout": "normal",
  "names": null,
  "mana_cost": "{1}{W}{W}",
  "cmc": 3,
  "colors": [
    "White"
  ],
  "color_identity": [
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
  "id": "d43016981b23288ff8faf3dfac674cdd1560a719"
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
    static constexpr std::array<color, 6> colors = {color::white, color::blue, color::black, color::red, color::green, color::colorless};

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
    std::string text;
    std::experimental::optional<std::string> power;
    std::experimental::optional<std::string> toughness;
    uint8_t converted_mana_cost{0};

    std::set<std::string> abilities;
    std::set<std::string> affinities;
};


std::string to_string(const card::type &t);
std::string to_string(const card::color &c);


void to_json(nlohmann::json &j, const card::color &c);
void to_json(nlohmann::json &j, const card::type &t);
void to_json(nlohmann::json &j, const card &p);

void from_json(const nlohmann::json &j, card &p);

} // namespace magique
