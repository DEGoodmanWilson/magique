#!/bin/bash

# download decklists
curl https://mtgdecks.net/decks/csv/Standard > ../../data/synergies/standard.csv
curl https://mtgdecks.net/decks/csv/Modern > ../../data/synergies/modern.csv
curl https://mtgdecks.net/decks/csv/Commander > ../../data/synergies/commander.csv
curl https://mtgdecks.net/decks/csv/Legacy > ../../data/synergies/legacy.csv
curl https://mtgdecks.net/decks/csv/Pauper > ../../data/synergies/pauper.csv
curl https://mtgdecks.net/decks/csv/Vintage > ../../data/synergies/vintage.csv
curl https://mtgdecks.net/decks/csv/Frontier > ../../data/synergies/frontier.csv
curl https://mtgdecks.net/decks/csv/Peasant > ../../data/synergies/peasant.csv
curl https://mtgdecks.net/decks/csv/Highlander > ../../data/synergies/highlander.csv

# analyse decklists
pip install -r requirements.txt
./install_punkt.py
# tags.txt is just for human inspection
./build_model.py ../../data > ../../data/synergies/tags.txt
./tag_cards.py ../../data > ../../data/synergies/card_tags.json
./find_tribal_cards.py ../../data > ../../data/synergies/card_tribes.json

# TODO use for loops here
./analyze_decks_by_ability.py ../../data standard > ../../data/synergies/conditional_probabilities_ability_standard.json
./analyze_decks_by_ability.py ../../data modern > ../../data/synergies/conditional_probabilities_ability_modern.json
./analyze_decks_by_ability.py ../../data commander > ../../data/synergies/conditional_probabilities_ability_commander.json
./analyze_decks_by_ability.py ../../data legacy > ../../data/synergies/conditional_probabilities_ability_legacy.json
./analyze_decks_by_ability.py ../../data pauper > ../../data/synergies/conditional_probabilities_ability_pauper.json
./analyze_decks_by_ability.py ../../data vintage > ../../data/synergies/conditional_probabilities_ability_vintage.json
./analyze_decks_by_ability.py ../../data frontier > ../../data/synergies/conditional_probabilities_ability_frontier.json
./analyze_decks_by_ability.py ../../data peasant > ../../data/synergies/conditional_probabilities_ability_peasant.json
./analyze_decks_by_ability.py ../../data highlander > ../../data/synergies/conditional_probabilities_ability_highlander.json

./analyze_decks_by_card.py ../../data standard > ../../data/synergies/conditional_probabilities_card_standard.json
./analyze_decks_by_card.py ../../data modern > ../../data/synergies/conditional_probabilities_card_modern.json
./analyze_decks_by_card.py ../../data commander > ../../data/synergies/conditional_probabilities_card_commander.json
./analyze_decks_by_card.py ../../data legacy > ../../data/synergies/conditional_probabilities_card_legacy.json
./analyze_decks_by_card.py ../../data pauper > ../../data/synergies/conditional_probabilities_card_pauper.json
./analyze_decks_by_card.py ../../data vintage > ../../data/synergies/conditional_probabilities_card_vintage.json
./analyze_decks_by_card.py ../../data frontier > ../../data/synergies/conditional_probabilities_card_frontier.json
./analyze_decks_by_card.py ../../data peasant > ../../data/synergies/conditional_probabilities_card_peasant.json
./analyze_decks_by_card.py ../../data highlander > ../../data/synergies/conditional_probabilities_card_highlander.json