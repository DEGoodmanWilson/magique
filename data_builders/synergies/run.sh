#!/bin/bash

# download decklists
curl https://mtgdecks.net/decks/csv/Standard > ../../data/synergies/standard.txt
curl https://mtgdecks.net/decks/csv/Modern > ../../data/synergies/modern.txt
curl https://mtgdecks.net/decks/csv/Commander > ../../data/synergies/commander.txt
curl https://mtgdecks.net/decks/csv/Legacy > ../../data/synergies/legacy.txt
curl https://mtgdecks.net/decks/csv/Pauper > ../../data/synergies/pauper.txt
curl https://mtgdecks.net/decks/csv/Vintage > ../../data/synergies/vintage.txt
curl https://mtgdecks.net/decks/csv/Frontier > ../../data/synergies/frontier.txt
curl https://mtgdecks.net/decks/csv/Peasant > ../../data/synergies/peasant.txt
curl https://mtgdecks.net/decks/csv/Highlander > ../../data/synergies/highlander.txt

# analyse decklists
pip install -r requirements.txt
./install_punkt.py
./build_model.py ../../data/AllCards.json ../../data/synergies
./tag_cards.py ../../data/AllCards.json ../../data/synergies > ../../data/synergies/card_tags.json
./card_synergies.py ../../data/AllCards.json ../../data/synergies/card_tags.json > ../../data/synergies/card_synergies.json
./analyze_decks.py ../../data/synergies/standard.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_standard.json
./analyze_decks.py ../../data/synergies/modern.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_modern.json
./analyze_decks.py ../../data/synergies/commander.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_commander.json
./analyze_decks.py ../../data/synergies/legacy.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_legacy.json
./analyze_decks.py ../../data/synergies/pauper.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_pauper.json
./analyze_decks.py ../../data/synergies/vintage.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_vintage.json
./analyze_decks.py ../../data/synergies/frontier.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_frontier.json
./analyze_decks.py ../../data/synergies/peasant.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_peasant.json
./analyze_decks.py ../../data/synergies/highlander.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_highlander.json