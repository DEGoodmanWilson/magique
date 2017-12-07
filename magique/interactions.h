//
// Created by Don Goodman-Wilson on 02/12/2017.
//

#pragma once

#include "catalog.h"
#include <unordered_map>

namespace magique
{

class interactions
{
public:
    interactions() = default;
    interactions(std::string filename);

    double evaluate(const card &a, const card &b) const;
    using interaction = std::unordered_map<std::string, double>;

private:


// An ability can affect the card itself, or other cards
// An ability can grant or take away powers from other cards
// An ability can interact with your own cards, or opponents' cards
// An ability can be permanent or ephemeral (this turn only, for example)
// An ability can have a number describing its cost or strength
// An ability can have an interaction strength with other abilities (linear, anti-linear; modular = no interaction)
// An ability can have an interaction strength with specific card types and subtypes (affects creatures, enchangments, non-land permanents, all permanents, land, dwarves, artificers, whatever)
// An ability can have a positive or a negative effect on the target

// for now let's start _simple_: Let's just record an ability's linearity with other abilities, that is, the degree that having _more_ of the same (or something else) is good or bad.

// This is a bad data structure for this, because it is recording relationships asymmetrically. But I am not sure that linearity is a symmetric relationship, TBH.
// For example, comparing a card with "Flying" and one with "Grants Flying". There is an anti-linear relationship, but is it reflexive?
// E.g. should the "Flying" creature get penalized for the presence of a "Grants Flying" instant, the other way around, or both?
// Indeed, I would argue that the presence of "Flying" creatures should penalize "Grants Flying", but not the other way around.
// (And the absence of "Flying" creatures should make it more attractive, but that's a different issue)
// So for now, I am OK with this data structure, but experience will surely dictate that we should do things differently in the future.

    // e.g. interactions["flying"]["flying"] = 1.0;

    std::unordered_map<std::string, std::shared_ptr<interaction>> interactions_store_;
};

} // namespace magique