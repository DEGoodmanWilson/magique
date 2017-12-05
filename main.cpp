#include <iostream>

#include "catalog.h"
#include "collection.h"
#include "card.h"
#include "deck.h"
#include "ga2/ga2.h"

using namespace magique;

int main()
{
    // fire up a catalog
    catalog master_catalog{"mtg.json", "annotations.json"};
    collection dons_collection{"don.csv", master_catalog};

    // get the interactions data
    interactions interactions{"interactions.json"};

    // pick a key card
    deck::add_key_card(master_catalog.at("Electrostatic Pummeler"));

    auto pop_size{1000};
    auto chromo_size = 30 - 13; //  30-card collection, with 12 lands and a key card specified
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
    pop.setReplacementSize(pop_size / 2);
    pop.setReplaceType(GA2_REPLACE_STEADYSTATE);
    pop.setSelectType(GA2_SELECT_ROULETTE);
    pop.setSort(true);
    pop.setEvalFunc([&](std::vector<ga2Gene> genes) -> double
                    {
                        //Consuct a deck from what we have here.
                        deck d{genes, dons_collection, interactions};
                        return d.evaluate();
                    });

    pop.init();
    pop.evaluate();
//    deck d{pop.getBestFitChromosome(), dons_collection};
//    auto rank = d.evaluate();
//    nlohmann::json j{d};
//    std::cout << "  " << pop.getMinFitness() << " " << pop.getAvgFitness() << " " << rank << " " << j.dump() << std::endl;

    for (auto gen = 0; gen < 1000; ++gen)
    {
        pop.select();
        pop.crossover();
        pop.mutate();
        pop.replace();
        pop.evaluate();
        std::cout << "." << std::flush;
        if((gen+1) % 100 == 0)     std::cout << std::endl;

//        deck d{pop.getBestFitChromosome(), dons_collection};
//        auto rank = d.evaluate();
//        nlohmann::json j{d};
//        std::cout << gen << " " << pop.getMinFitness() << " " << pop.getAvgFitness() << " " << rank << " " << j.dump() << std::endl;
    }

    std::cout << std::endl;

    deck d{pop.getBestFitChromosome(), dons_collection, interactions};
    auto rank = d.evaluate();
    nlohmann::json j{d};
    std::cout << j.dump(4) << std::endl;



///// The code below is just to demonstrate how you can use the evaluation function to evaluate existing decks you might already have.

//    deck dons_wu_flying{"BW Flying.txt", master_catalog, interactions};
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
