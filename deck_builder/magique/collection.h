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
    collection() = default;

    collection(std::string path, std::string filename, catalog *catalog);

    card *at(uint64_t loc);

    uint64_t index_at(std::string name, int64_t past_index = -1);

    uint64_t count();

private:
    catalog *catalog_; // non-owning pointer
    std::vector<std::string> cards_;
};

}
