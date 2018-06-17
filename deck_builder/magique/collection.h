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
    collection(std::string path, std::string filename, const catalog &catalog);
    card at(uint64_t loc) const;
    uint64_t index_at(std::string name, int64_t past_index=-1) const;
    uint64_t count();
private:
    const catalog &catalog_;
    std::vector<std::string> cards_;
};

}
