//
// Created by Donald Goodman-Wilson on 16/09/2018.
//

#include "synergies.h"
#include <iostream>
#include <fstream>

namespace magique
{
namespace evaluators
{

void load_synergies(std::string path, const magique::catalog &catalog)
{
    std::cerr << "Loading tag data...";
    std::ifstream ifs{path + "/synergies/card_tags.json"};
    nlohmann::json mechanics;
    ifs >> mechanics;
    ifs.close();
    std::cerr << "done." << std::endl;
}

evaluation synergies(const card *card_a, const card *card_b, const card::format format)
{
    // TODO
    return {1.0, 1.0, "synergies"};
}

}}