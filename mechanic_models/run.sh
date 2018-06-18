#!/bin/bash

./install_punkt.py
./build_model.py > ../data/mechanics.txt
./tag_cards.py > ../data/card_tags.json
./analyze_decks.py ../data/Standard.txt > ../data/conditional_probabilities_standard.json
./analyze_decks.py ../data/Modern.txt > ../data/conditional_probabilities_modern.json
./analyze_decks.py ../data/Commander.txt > ../data/conditional_probabilities_commander.json
./analyze_decks.py ../data/Legacy.txt > ../data/conditional_probabilities_legacy.json
./analyze_decks.py ../data/Pauper.txt > ../data/conditional_probabilities_pauper.json
./analyze_decks.py ../data/Vintage.txt > ../data/conditional_probabilities_vintage.json
./analyze_decks.py ../data/Frontier.txt > ../data/conditional_probabilities_frontier.json
./analyze_decks.py ../data/Peasant.txt > ../data/conditional_probabilities_peasant.json
./analyze_decks.py ../data/Highlander.txt > ../data/conditional_probabilities_highlander.json