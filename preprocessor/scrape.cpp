//
// Created by Don Goodman-Wilson on 25/11/2017.
//

#include <regex>
#include <json.hpp>
#include <cpr/cpr.h>

int main(void)
{
    std::string next_url{"https://api.magicthegathering.io/v1/cards"};
    bool done{false};

    std::cout << "[" << std::endl;

    while(!done)
    {
        auto result = cpr::Get(cpr::Url{next_url});

        std::string remaining{result.header["ratelimit-remaining"]};
        if(remaining == "0")
        {
            std::cerr << "rate limited! Keep trying!" << std::endl;
            done = true;
        }

        std::string link{result.header["Link"]};
//        std::string link{R"(<https://api.magicthegathering.io/v1/cards?page=1>; rel="first", <https://api.magicthegathering.io/v1/cards?page=349>; rel="last", <https://api.magicthegathering.io/v1/cards?page=250>; rel="next", <https://api.magicthegathering.io/v1/cards?page=248>; rel="prev")"};
        std::regex next_link_regex{R"(<(https:\/\/api\.magicthegathering\.io\/v1\/cards\?page=[0-9]+)>; rel="next")"};
        std::smatch next_link_match;
        if(std::regex_search(link, next_link_match, next_link_regex)) {
            next_url = next_link_match[1];
            std::cerr << next_url << std::endl;
        }
        else
        {
            std::cerr << "DONE!" << std::endl;
            done = true;
        }

        nlohmann::json temp = nlohmann::json::parse(result.text);
        for(auto card : temp["cards"])
        {
            std::cout << card.dump(4) << "," << std::endl;
        }
    }

    std::cout << '\b' << '\b' << std::endl << "]" << std::endl;
}