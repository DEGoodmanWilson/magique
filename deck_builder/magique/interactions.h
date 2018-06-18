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
    interactions(std::string path);

    double evaluate(const card &a, const card &b) const;
private:
    using interaction = std::unordered_map<int, double>;
    std::unordered_map<int, interaction> interactions_store_;
};

} // namespace magique