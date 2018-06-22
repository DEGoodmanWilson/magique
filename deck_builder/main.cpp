#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "docopt/docopt.h"
#include "ezETAProgressBar/ezETAProgressBar.hpp"

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


static const char USAGE[] =
        R"(magique.

    Usage:
      magique <data-pathname> <collection-filename> [options] [-k <card>]...

    Options:
      -h --help     Show this screen.
      --version     Show version.
      -p <size> --population=<size>  Set initial population size [default: 1000]
      -g <count> --generations=<count>  Set the number of generations to run [default: 1000]
      -k <card> --key_card=<card>  Set one or more key cards that must be included in the deck
      -t <threads> --thread_num=<threads>  Set the number of threads to use, use 0 to indicate enough threads to saturate available CPUs [default: 0]
      -c <colors> --colors=<colors>  Set the number of desired colors to use in the deck, from 1 to 5. [default: 2]
      -i <color_identity> --color_identity=<color_identity>  Set the color identity of the deck, using a string like "wubgr". Default value lets algorithm decide. [default: ""]
      -d <deck_size> --deck_size=<deck_size>  The number of non-land cards to use. [default: 34]
)";


int main(int argc, char **argv)
{
    // set up signal handlers
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);


    // parse CLI options
    std::map<std::string, docopt::value> args
            = docopt::docopt(USAGE,
                             {argv + 1, argv + argc},
                             true,               // show help if requested
                             "magique 1.0.0");  // version string


    std::string data_pathname;
    std::string collection_filename;
    int pop_size;
    uint64_t generations;
    std::vector<std::string> key_cards;
    uint32_t thread_num;
    uint8_t colors;
    uint16_t deck_size;
    std::unordered_set<card::color> color_identity;

    for (auto const &arg : args)
    {
        if (arg.first == "--generations")
        { generations = arg.second.asLong(); }
        else if (arg.first == "--population")
        { pop_size = arg.second.asLong(); }
        else if (arg.first == "<data-pathname>")
        { data_pathname = arg.second.asString(); }
        else if (arg.first == "<collection-filename>")
        { collection_filename = arg.second.asString(); }
        else if (arg.first == "--key_card")
        { key_cards = arg.second.asStringList(); }
        else if (arg.first == "--thread_num")
        {
            thread_num = arg.second.asLong();
            if (thread_num == 0)
            {
                thread_num = std::thread::hardware_concurrency() - 1;
            }
            else
            {
                thread_num--; // 1 thread means no additional threads.
            }
        }
        else if (arg.first == "--colors")
        { colors = arg.second.asLong(); }
        else if (arg.first == "--color_identity")
        {
            auto id_str = arg.second.asString();
            auto id_cstr = id_str.c_str();
            for(int i = 0; i < id_str.length(); ++i)
            {
                switch(id_cstr[i])
                {
                    case 'w':
                    case 'W':
                        color_identity.insert(card::color::white);
                        break;
                    case 'u':
                    case 'U':
                        color_identity.insert(card::color::blue);
                        break;
                    case 'b':
                    case 'B':
                        color_identity.insert(card::color::black);
                        break;
                    case 'r':
                    case 'R':
                        color_identity.insert(card::color::red);
                        break;
                    case 'g':
                    case 'G':
                        color_identity.insert(card::color::green);
                }
            }
        }
        else if (arg.first == "--deck_size") deck_size = arg.second.asLong();
    }



    // set deck evaluation options
    if(colors < color_identity.size())
        colors = color_identity.size();

    deck::colors = colors;
    deck::color_identity = color_identity;
    deck::deck_minimum = deck_size;

    // fire up a catalog
    catalog master_catalog{data_pathname};

    // load up the user's personal collection
    collection dons_collection{data_pathname, collection_filename, master_catalog};

    // get the interactions data
    interactions interactions{data_pathname};

    // pick a key card
    for (const auto &card : key_cards)
    {
        try
        {
            deck::add_key_card(master_catalog.at(card));
        }
        catch (...)
        {
            std::cerr << "No such card: " << card << std::endl;
            exit(1);
        }
    }

    double wiggle = deck_size * 0.10;
    if (wiggle < 1.0) wiggle = 1.0;
    auto chromo_size = deck_size + static_cast<uint16_t>(wiggle); //  add 10% extra for the GA
    ga2Population pop{pop_size, chromo_size, thread_num};
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
    pop.setCrossoverType(GA2_CROSSOVER_UNIFORM);
    pop.setInteger(true);
    pop.setReplacementSize(pop_size / 2);
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

    ez::ezETAProgressBar eta(generations);
    eta.start();

    for (auto gen = 0; gen < generations; ++gen)
    {
        pop.select();
        pop.crossover();
        pop.mutate();
        pop.replace();
        pop.evaluate();

        ++eta;

        if (dump_and_abort)
        {
            // If we get a sigabort signal, stop here and dump the current best-fit chromosome. Some people are just impatient
            eta.done();
            deck d{pop.getBestFitChromosome(), dons_collection, interactions};
            auto rank = d.evaluate();
            nlohmann::json j{d};
            std::cout << j.dump(4) << std::endl;
            exit(1);
        }
    }

    eta.done();

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
