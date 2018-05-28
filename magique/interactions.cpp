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
    nlohmann::json interactions;
    ifs >> interactions;
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

        // in order to find abilities that interact with the _lack_ of an ability or type, we need to iterate through the interactions first
        for (const auto &interaction : *interactions)
        {
            bool negative{interaction.first[0] == '!'};

            auto b_interaction = interaction.first;
            if (negative)
            {
                b_interaction = b_interaction.substr(1);
            }

            bool b_has_interaction{b.abilities.count(b_interaction) || b.has_type(b_interaction) || b.subtypes.count(b_interaction)};

            // if b has this ability or type…or it is an interaction with the absence of an ability or type, and b doesn't have it
            if ((!negative && b_has_interaction) ||
                (negative && !b_has_interaction))
            {
                value += interaction.second;
            }
        }
    }

    //iterate through card a's affinities, and see if card b satisfies them
    for (const auto &aff: a.affinities)
    {
        double score{1.0};
        std::string affinity{aff};
        if (affinity[0] == '!') // handle negative affinities
        {
            affinity = affinity.substr(1);
            score = -1.0;
        }

        // check against b's abilities, types, and subtypes
        if (b.abilities.count(affinity) || b.has_type(affinity) || b.subtypes.count(affinity))
        {
            value += score;
        }
    }

    return value;
}

} // namespace magique