//
// Created by Don Goodman-Wilson on 29/06/2018.
//

#include "power_toughness.h"

namespace magique
{
namespace evaluators
{


evaluation eval_power(const card &card, const uint16_t count, const card::format format)
{
    static const auto EVALUATOR_REASON_POWER{"Power"};
    static const auto MAX_POWER{8};
    double pow_factor{0};

    if (card.power)
    {
        if (*card.power == "*")
        {
            // Assume cards with pow '*' are of max power
            // because why not. We don't know what the power actually is, but ideally we can get the card pretty strong.
            pow_factor = MAX_POWER;
        }
        else
        {
            // Assume that creature power can range from 0 to 8. This is not a true assumption, of course, but we need an upper bound
            pow_factor = stoul(*card.power);
            if (pow_factor > MAX_POWER) pow_factor = MAX_POWER;
        }

        return {pow_factor, MAX_POWER, EVALUATOR_REASON_POWER};
    }

    return {0, MAX_POWER, EVALUATOR_REASON_POWER};
}


evaluation eval_toughness(const card &card, const uint16_t count, const card::format format)
{
    static const auto EVALUATOR_REASON_TOUGHNESS{"Toughness"};
    static const auto MAX_TOUGHNESS{8};
    double tough_factor{0};

    if (card.toughness)
    {
        if (*card.toughness == "*")
        {
            // Assume cards with pow '*' are of max toughness
            // because why not. We don't know what the toughness actually is, but ideally we can get the card pretty strong.
            tough_factor = MAX_TOUGHNESS;
        }
        else
        {
            // Assume that creature toughness can range from 0 to 8. This is not a true assumption, of course, but we need an upper bound
            tough_factor = stoul(*card.toughness);
            if (tough_factor > MAX_TOUGHNESS) tough_factor = MAX_TOUGHNESS;
        }

        return {tough_factor, MAX_TOUGHNESS, EVALUATOR_REASON_TOUGHNESS};
    }

    return {0, MAX_TOUGHNESS, EVALUATOR_REASON_TOUGHNESS};
}

}
}