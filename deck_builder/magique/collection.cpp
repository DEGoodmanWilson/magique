//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "collection.h"
#include "../TextUtils/TextUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace magique
{

// TODO find a way to have case_insensitive std::unordered_map, instead of this ::tolower BS


collection::collection(std::string path, std::string filename, catalog *catalog) : catalog_{catalog}
{
    //load up from a CSV
    std::cerr << "Loading collection data...";

    std::ifstream ifs{path + "/" + filename};
    nlohmann::json collection_json;
    ifs >> collection_json;
    ifs.close();

    //    for (auto& [key, value] : collection_json.items())
    for (auto& kv : collection_json.items())
    {
        std::string name{kv.key()};
        uint16_t count{kv.value().get<decltype(count)>()};;
        bool is_land{false};


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
    std::cerr << "done." << std::endl;
}

card *collection::at(uint64_t loc)
{
//    std::cout << (int)loc << " " << cards_.size() << " " << cards_[loc] << std::endl;
    return catalog_->at(cards_[loc]);
}

uint64_t collection::index_at(std::string name, const int64_t index_past)
{
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