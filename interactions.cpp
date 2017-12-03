//
// Created by Don Goodman-Wilson on 02/12/2017.
//

#include "interactions.h"
#include <fstream>

namespace magique
{

interactions::interactions(std::string filename)
{
    std::ifstream ifs(filename);
    nlohmann::json interactions{ifs};
    ifs.close();

    // ok what we have is an object of interactions. The key is the interaction name.
    for (auto it = interactions.begin(); it != interactions.end(); ++it)
    {
        //key is a string, value is an object mapping strings to doubles
        interactions_store_[it.key()] = std::make_shared<interaction>(it.value().get<std::unordered_map<std::string, double>>());
    }

}

double interactions::evaluate(const card &a, const card &b) const
{
    double value{0.0};

    //iterate through card a's abilities
    for (const auto &ability : a.abilities)
    {
        //find the interactions for this ability
        if (interactions_store_.count(ability) == 0) continue;

        auto interactions = interactions_store_.at(ability);

        // in order to find abilities that interact with the _lack_ of an ability, we need to iterate through the interactions first
        for (const auto &interaction : *interactions)
        {
            bool negative{interaction.first[0] == '!'};

            auto b_ability = interaction.first;
            if (negative)
            {
                b_ability = b_ability.substr(1);
            }
            bool b_has_ability{b.abilities.count(b_ability) != 0};

            // if b has this ability…or it is an interaction with the absence of an ability, and b doesn't have it
            if ((!negative && b_has_ability) ||
                (negative && !b_has_ability))
            {
                value += interaction.second;
            }
        }
    }

    return value;
}

} // namespace magique