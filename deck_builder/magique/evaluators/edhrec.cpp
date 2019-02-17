//
// Created by Donald Goodman-Wilson on 2019-02-17.
//

#include "edhrec.h"

namespace magique
{
namespace evaluators
{

evaluation edhrec(const card *card_a, const card *card_b, const card::format format)
{
    return {1.0, 1.0, "edhrec"};
}

}}