//
// Created by Donald Goodman-Wilson on 2019-02-17.
//

#pragma once

#include "types.h"

namespace magique
{
namespace evaluators
{

void load_edhrec(std::string path);

evaluation edhrec_price(const card *card, const uint16_t count, const card::format format);
evaluation edhrec_rank(const card *card, const uint16_t count, const card::format format);
evaluation edhrec_decks(const card *card, const uint16_t count, const card::format format);

evaluation edhrec_synergy(const card *card_a, const card *card_b, const card::format format);

}
}
