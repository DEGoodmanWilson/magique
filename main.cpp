#include <iostream>

#include <luna/luna.h>
#include "logger.h"
#include "catalog.h"
#include "collection.h"
#include "card.h"
#include "deck.h"
#include "ga2/ga2.h"

using namespace magique;

int main()
{
    // set up the loggers
    luna::set_access_logger(access_logger);
    luna::set_error_logger(error_logger);

    // determine which port to run on, default to 8080
    auto port = 8080;
    if (auto port_str = std::getenv("PORT"))
    {
        try
        {
            port = std::atoi(port_str);
        }
        catch (const std::invalid_argument &e)
        {
            error_logger(luna::log_level::FATAL, "Invalid port specified in env $PORT.");
            return 1;
        }
        catch (const std::out_of_range &e)
        {
            error_logger(luna::log_level::FATAL, "Port specified in env $PORT is too large.");
            return 1;
        }
    }


    // fire up a catalog
    catalog master_catalog{"mtg.json"};

    collection dons_collection{"don.csv", master_catalog};

//    std::cout << dons_collection.at(0).name << std::endl;
    auto pop_size{10000};
    auto chromo_size = 30 - 12; //  30-card collection, with  12 lands
    ga2Population pop{pop_size, chromo_size};
    std::vector<ga2Gene> min, max;
    for (auto i = 0; i < chromo_size; ++i)
    {
        min.push_back(0);
        max.push_back(dons_collection.count());
    }
    pop.setMinRanges(min);
    pop.setMaxRanges(max);
    pop.setMutationRate(0.1); //TODO decrease over time.
    pop.setCrossoverRate(0.01);
    pop.setCrossoverType(GA2_CROSSOVER_ONEPOINT);
    pop.setInteger(true);
    pop.setReplacementSize(pop_size/2);
    pop.setReplaceType(GA2_REPLACE_STEADYSTATE);
    pop.setSelectType(GA2_SELECT_ROULETTE);
    pop.setSort(true);
    pop.setEvalFunc([&](std::vector<ga2Gene> genes) -> double
                    {
                        //Consuct a deck from what we have here.
                        deck d{genes, dons_collection};
                        return d.eval();
                    });

    pop.init();
    pop.evaluate();
    deck d{pop.getBestFitChromosome(), dons_collection};
    auto rank = d.eval();
    nlohmann::json j{d};
    std::cout << pop.getAvgFitness() << " " << rank << " " << j.dump() << std::endl;

    for(auto gen = 0; gen < 10000; ++gen)
    {
        pop.select();
        pop.crossover();
        pop.mutate();
        pop.replace();
        pop.evaluate();
        deck d{pop.getBestFitChromosome(), dons_collection};
        auto rank = d.eval();
        nlohmann::json j{d};
        std::cout << gen << " " << pop.getAvgFitness() << " " << rank << " " << j.dump() << std::endl;
    }




    deck dons_wu_flying{"BW Flying.txt", master_catalog};
    dons_wu_flying.eval();

    nlohmann::json deck_j;
    to_json(deck_j, dons_wu_flying);

    std::cout << "        " << deck_j["rank"].dump() << " " << deck_j.dump() << std::endl;

//    // add endpoints
//    luna::router api{"/"};
//    api.set_mime_type("text/json");
//
//    // For now, let's just define them inline, we can move them out later.
//    api.handle_request(luna::request_method::GET, R"(/(.+))", [&](luna::request request) -> luna::response
//    {
//        try
//        {
//            auto card = master_catalog.at(request.matches[1]);
//            nlohmann::json j = card;
//            return {j.dump()};
//        }
//        catch(...)
//        {
//            return {404};
//        }
//    });
//
//
//    // launch server
//    luna::server server;
//
//    server.add_router(api);
//
//    if (!server.start(port))
//    {
//        return 1;
//    }


    return 0;
}