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

void load_synergies(std::string path, const magique::catalog &catalog);

evaluation synergies(const card *card_a, const card *card_b, const card::format format);

}
}