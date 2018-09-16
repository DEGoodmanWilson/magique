//
// Created by Donald Goodman-Wilson on 16/09/2018.
//

#include "interactions.h"

namespace magique
{
namespace evaluators
{

evaluation interactions(const card &card_a, const card &card_b, const card::format format)
{
    return {1.0, 1.0, "interactions"};
}

}}