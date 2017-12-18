#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "magique/catalog.h"
#include "magique/collection.h"
#include "magique/card.h"
#include "magique/deck.h"
#include "ga2/ga2.h"

using namespace magique;

bool dump_and_abort{false};

void signal_handler(int s)
{
    dump_and_abort = true; //not gonna worry about sync because bools are atomic writable
}

int main()
{
    // set up signal handlers
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);


    // fire up a catalog
    catalog master_catalog{"data/catalog.json", "data/annotations.json"};

    // load up the user's personal collection
    collection dons_collection{"data/collection_dons.csv", master_catalog};

    // get the interactions data
    interactions interactions{"data/interactions.json"};

    // pick a key card
//    deck::add_key_card(master_catalog.at("Electrostatic Pummeler"));

    auto pop_size{1000};
    auto chromo_size = 60 - 24; //  30-card collection, with 12 lands and a key card specified
    ga2Population pop{pop_size, chromo_size};
    std::vector<ga2Gene> min, max;
    for (auto i = 0; i < chromo_size; ++i)
    {
        min.push_back(0);
        max.push_back(dons_collection.count() - 1);
    }
    pop.setMinRanges(min);
    pop.setMaxRanges(max);
    pop.setMutationRate(0.2); //TODO decrease over time.
    pop.setCrossoverRate(1.0);
    pop.setCrossoverType(GA2_CROSSOVER_ONEPOINT);
    pop.setInteger(true);
    pop.setReplacementSize(pop_size/2);
    pop.setReplaceType(GA2_REPLACE_STEADYSTATE);
    pop.setSelectType(GA2_SELECT_ROULETTE);
    pop.setSort(true);
    pop.setEvalFunc([&](std::vector<ga2Gene> genes) -> double
                    {
                        deck d{genes, dons_collection, interactions};
                        return d.evaluate();
                    });

    pop.init();
    pop.evaluate();

    for (auto gen = 0; gen < 1000; ++gen)
    {
        pop.select();
        pop.crossover();
        pop.mutate();
        pop.replace();
        pop.evaluate();
        std::cout << "." << std::flush;
//        std::cout << pop.getMinFitness() << " : " << pop.getAvgFitness() << " : " << pop.getMaxFitness() << std::endl;
//        deck d{pop.getBestFitChromosome(), dons_collection, interactions};
//        auto genes = pop.getBestFitChromosome();
//        deck d{genes, dons_collection, interactions};
//        nlohmann::json j{d};
//        std::cout << j.dump(4) << std::endl;
        if ((gen + 1) % 100 == 0) std::cout << std::endl;

        if(dump_and_abort)
        {
            // If we get a sigabort signal, stop here and dump the current best-fit chromosome. Some people are just impatient
            deck d{pop.getBestFitChromosome(), dons_collection, interactions};
            auto rank = d.evaluate();
            nlohmann::json j{d};
            std::cout << j.dump(4) << std::endl;
            exit(1);
        }
    }

    std::cout << std::endl;

    deck d{pop.getBestFitChromosome(), dons_collection, interactions};
    auto rank = d.evaluate();
    nlohmann::json j{d};
    std::cout << j.dump(4) << std::endl;



///// The code below is just to demonstrate how you can use the evaluation function to evaluate existing decks you might already have.

//    deck dons_wu_flying{"data/sample decks/BW Flying.txt", master_catalog, interactions};
//    dons_wu_flying.evaluate();
//    nlohmann::json deck_j;
//    to_json(deck_j, dons_wu_flying);
//    std::cout << "        " << deck_j["rank"].dump() << " " << deck_j.dump() << std::endl;
//
//    deck bad{{12, 13, 12, 13, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5}, dons_collection, interactions};
//    bad.evaluate();
//    nlohmann::json deck_bad;
//    to_json(deck_bad, bad);
//    std::cout << "        " << deck_bad["rank"].dump() << " " << deck_bad.dump() << std::endl;



    return 0;
}
