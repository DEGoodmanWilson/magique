//
// Created by Don Goodman-Wilson on 02/12/2017.
//

#include "interactions.old.h"
#include <fstream>
#include <vector>

namespace magique
{

const std::vector<std::string> explode_(const std::string &s, const char &c)
{
    std::string buff{""};
    std::vector<std::string> v;

    for (auto n:s)
    {
        if (n != c)
        { buff += n; }
        else if (n == c && buff != "")
        {
            v.push_back(buff);
            buff = "";
        }
    }
    if (buff != "") v.push_back(buff);

    return v;
}

interactions::interactions(std::string path) : interactions_store_{}
{
    // TODO let's not just hard code Standard.
    std::ifstream ifs(path + "/conditional_probabilities_standard.json");
    nlohmann::json interactions;
    ifs >> interactions;
    ifs.close();

    // TODO these should go with _cards_ not this stupid table!
    ifs = std::ifstream(path + "/card_affinities.json");
    nlohmann::json affinities;
    ifs >> affinities;
    ifs.close();

    // ok what we have is an object of interactions. The key is a pair of numbers, the keys into our map.
    // TODO make this format easier to parse!
    for (auto it = interactions.begin(); it != interactions.end(); ++it)
    {
        //key is a string of two ints with a |, e.g. "1|2", value is a double.
        auto keys = explode_(it.key(), '|');
        auto m = std::atoi(keys[0].c_str());
        auto n = std::atoi(keys[1].c_str());

        interactions_store_[m][n] = it.value().get<double>();
    }

    affinities_store_ = affinities.get<decltype(affinities_store_)>();
}

uint64_t interactions::evaluate_interactions(const card &a, const card &b) const
{
    // Iterate over all of card a's abilities, and b's abilities, and look up the conditional probabilities
    // SUM _0..m, 0..n p(a_m | b_n)

    uint64_t value{0};

    for (const auto &m : a.mechanics)
    {
        for (const auto &n : b.mechanics)
        {
            value += interactions_store_[m][n] * 100000;
        }
    }

    // TODO Should we normalize the score so that cards with _more_ mechanics don't outshine cards with fewer?
    if (a.mechanics.size() && b.mechanics.size())
    {
        value = value / (a.mechanics.size() * b.mechanics.size());
    }

    return value / 100000;
}

uint64_t interactions::evaluate_affinities(const card &a, const card &b) const
{
    // Iterate over all of card a's abilities, and b's abilities, and look up the conditional probabilities
    // SUM _0..m, 0..n p(a_m | b_n)

    uint64_t value{0};


    if (affinities_store_.count(a.name) > 0)
    {
        // Get the affinities card a's rules, and any of card b's types
        for (auto affinity : affinities_store_.at(a.name))
        {
            // TODO This is ridicolously ineffecient. Is there any better way to do this?
            if (b.subtypes.count(affinity) > 0)
            {
                value += 1;
            } //TODO how much!? Shouldnt this be normalized to the size of the deck?
        }

        //TODO What else should we be on the lookout for? For whether a and b _share_ affinities?
    }


    return value;
}


} // namespace magique