//
// Created by Donald Goodman-Wilson on 2019-02-17.
//

#include "edhrec.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

namespace magique
{
namespace evaluators
{

static YAML::Node edhrec_data;

void load_edhrec(std::string path)
{
    std::cerr << "Loading EDHREC data..." << std::endl;
    edhrec_data = YAML::LoadFile(path + "/card_tags.json");
}

evaluation edhrec(const card *card_a, const card *card_b, const card::format format)
{
    return {1.0, 1.0, "edhrec"};
}

}}