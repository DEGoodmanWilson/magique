//
// Created by Don Goodman-Wilson on 29/06/2018.
//

#pragma once

#include "types.h"

namespace magique
{
namespace evaluators
{

evaluation eval_power(const card &card, const uint16_t count, const card::format format);

evaluation eval_toughness(const card &card, const uint16_t count, const card::format format);

}
}