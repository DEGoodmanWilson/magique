//
// Created by Don Goodman-Wilson on 29/06/2018.
//

#pragma once

#include <string>
#include "../card.h"

namespace magique
{
namespace evaluators
{

struct evaluation
{
    double score;
    double scale; // what to divide the score by to get a number between 0 and 1. We only want to divide once, at the end
    std::string reason;
};

using card_evaluator = std::function<evaluation(const card&, const uint16_t, const card::format)>;
using card_pair_evaluator = std::function<evaluation(const card&, const card&, const card::format)>;

}
}