//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#pragma once

#include <string>
#include <vector>
#include "catalog.h"

namespace magique
{

class collection
{
public:
    collection(std::string filename, const catalog &catalog);

    card at(uint8_t loc) const;
    uint8_t count();
private:
    const catalog &catalog_;
    std::vector<std::string> cards_;
};

}
