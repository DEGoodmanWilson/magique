//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#pragma once

#include <string>
#include <vector>

namespace magique
{

class collection
{
public:
    collection(std::string filename);
private:
    std::vector<std::string> cards_;
};

}
