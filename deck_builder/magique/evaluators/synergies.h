//
// Created by Donald Goodman-Wilson on 16/09/2018.
//

#pragma once

#include "types.h"
#include "../catalog.h"

namespace magique
{
namespace evaluators
{

void load_synergies(std::string path, magique::catalog &catalog, magique::card::format format);

evaluation card_synergy(const card *card_a, const card *card_b, card::format format);
evaluation mechanic_synergy(const card *card_a, const card *card_b, card::format format);
evaluation tribal_synergy(const card *card_a, const card *card_b, card::format format);
}
}