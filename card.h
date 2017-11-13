//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#pragma once

#include <string>
#include <vector>
#include <locale>
#include <json.hpp>
#include <luna/optional.hpp>

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
        white = 0,
        blue = 1,
        black = 2,
        red = 3,
        green = 4,
        colorless = 5,
    };

    enum class type
    {
        land = 0,
        creature = 1,
        artifact = 2,
        enchantment = 3,
        planeswalker = 4,
        instant = 5,
        sorcery = 6,
    };

    static std::vector<color> color_identities_from_array(std::vector<std::string> arr);

    static std::vector<type> types_from_string(std::string str);

    std::string id;
    std::string name;
    std::vector<type> types;
    std::vector<color> color_identity;
    std::string text;
    std::experimental::optional<std::string> power;
    std::experimental::optional<std::string> toughness;
    uint8_t converted_mana_cost;
};

void to_json(nlohmann::json &j, const card &p);

void from_json(const nlohmann::json &j, card &p);

} // namespace magique