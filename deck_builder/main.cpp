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
#include <ga3/ga3.hpp>

#include "magique/evaluators/power_toughness.h"

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
      -p <size> --population=<size>  Set initial population size [default: 10]
      -g <count> --generations=<count>  Set the number of generations to run [default: 10]
      -k <card> --key_card=<card>  Set one or more key cards that must be included in the deck
      -s --single_threaded    Run in single-threaded mode
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
    uint64_t pop_size;
    uint64_t generations;
    std::vector<std::string> key_cards;
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
        else if (arg.first == "--single_threaded")
        {
            std::cout << "Using single-threaded mode" << std::endl;
            ga3::population::single_threaded = true;
        }
        else if (arg.first == "--colors")
        { colors = arg.second.asLong(); }
        else if (arg.first == "--color_identity")
        {
            auto id_str = arg.second.asString();
            auto id_cstr = id_str.c_str();
            for (int i = 0; i < id_str.length(); ++i)
            {
                switch (id_cstr[i])
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
    if (colors < color_identity.size())
    {
        colors = color_identity.size();
    }

    deck::colors = colors;
    deck::color_identity = color_identity;
    deck::deck_minimum = deck_size;

    deck::add_evaluator(magique::evaluators::eval_power);
    deck::add_evaluator(magique::evaluators::eval_toughness);

    // fire up a catalog
    catalog master_catalog{data_pathname};

    // load up the user's personal collection
    deck::collection = collection{data_pathname, collection_filename, &master_catalog};

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
    std::vector<ga3::gene_range> ranges;
    for (auto i = 0; i < chromo_size; ++i)
    {
        ranges.emplace_back(0, deck::collection.count() - 1);
    }

    ga3::population
            pop{pop_size, ranges, [&](const auto &genes) -> double
    {
        deck d{genes};
        return d.evaluate();
//        return 1.0;
    }, ga3::population::mutation_rate{0.2},
                ga3::population::replacement_rate{0.5},
                ga3::population::replacement_kind_t::steady_state,
                ga3::population::selection_kind_t::ranked,
                ga3::chromosome::crossover_kind_t::uniform
    };

//    pop.setMinRanges(min);
//    pop.setMaxRanges(max);
//    pop.setMutationRate(0.2); //TODO decrease over time.
//    pop.setCrossoverRate(1.0);
//    pop.setCrossoverType(GA2_CROSSOVER_UNIFORM);
//    pop.setInteger(true);
//    pop.setReplacementSize(pop_size / 2);
//    pop.setReplaceType(GA2_REPLACE_STEADYSTATE);
//    pop.setSelectType(GA2_SELECT_ROULETTE);
//    pop.setSort(true);
//    pop.setEvalFunc([&](const std::vector<ga2Gene> &genes) -> double
//                    {
////                        deck d{genes};
////                        return d.evaluate();
//                        return 1.0;
//                    });
//
//    pop.init();
//    pop.evaluate();

    ez::ezETAProgressBar eta(generations);
    eta.start();

    pop.evolve(generations, [&]()
    {
        ++eta;

        if (dump_and_abort)
        {
            // If we get a sigabort signal, stop here and dump the current best-fit chromosome. Some people are just impatient
            eta.done();
            auto best_fit = pop.evaluate();
            deck d{best_fit.get_genes()};
            auto rank = best_fit.get_fitness();
            nlohmann::json j{d};
            std::cout << j.dump(4) << std::endl;
            exit(1);
        }

    });
//        pop.select();
//        pop.crossover();
//        pop.mutate();
//        pop.replace();
//        pop.evaluate();

    eta.done();

    auto best_fit = pop.evaluate();
    deck d{best_fit.get_genes()};
    auto rank = best_fit.get_fitness();
    nlohmann::json j{d};
    std::cout << j.dump(4) << std::endl;

    return 0;
}
