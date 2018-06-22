//
// Created by Don Goodman-Wilson on 02/12/2017.
//

#pragma once

#include "catalog.h"
#include <array>

namespace magique
{

class interactions
{
public:
    interactions(std::string path);

    uint64_t evaluate(const card &a, const card &b) const;
private:
    std::array<std::array<double, 250>, 250> interactions_store_; // TODO hard-coded number of classes, ugh.
};

} // namespace magique