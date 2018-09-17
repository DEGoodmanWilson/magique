//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "collection.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace magique
{

// TODO find a way to have case_insensitive std::unordered_map, instead of this ::tolower BS


collection::collection(std::string path, std::string filename, catalog *catalog) : catalog_{catalog}
{
    //load up from a CSV
    std::ifstream file{path + "/" + filename};

    std::string row;
    getline(file, row); //throw away header row;
    while (getline(file, row))
    {
        std::istringstream tokenS(row);
        std::string count_str;
        uint16_t count;
        std::string name;
        getline(tokenS, count_str, ',');
        count = stoul(count_str);
        getline(tokenS, name, '"'); //clear first " char
        getline(tokenS, name, '"');
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);

//        name = name.substr(1,name.length()-1);

        bool is_land = false;
        for (auto type : catalog->at(name)->types)
        {
            if (type == card::type::land || type == card::type::basic_land)
            {
                is_land = true;
                break;
            }
        }
        if (!is_land)
        {
            while (count)
            {
                cards_.push_back(name);
                --count;
            }
        }
    }


}

card *collection::at(uint64_t loc)
{
//    std::cout << (int)loc << " " << cards_.size() << " " << cards_[loc] << std::endl;
    return catalog_->at(cards_[loc]);
}

uint64_t collection::index_at(std::string name, const int64_t index_past)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    int64_t i{0};
    for (i = 0; i < cards_.size(); ++i)
    {
        if ((cards_[i] == name) && (i > index_past))
        {
            return i;
        }
    }

    std::out_of_range e{"No card " + name};
    throw (e);
}

uint64_t collection::count()
{
    return cards_.size();
}

} // namespace magique