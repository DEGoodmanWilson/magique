//
// Created by Don Goodman-Wilson on 02/12/2017.
//

#include "interactions.h"
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

}

uint64_t interactions::evaluate(const card &a, const card &b) const
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

    return value;
}

} // namespace magique