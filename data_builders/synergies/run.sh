#!/bin/bash

# download decklists
curl https://mtgdecks.net/decks/csv/Standard > ../../data/synergies/Standard.txt
curl https://mtgdecks.net/decks/csv/Modern > ../../data/synergies/Modern.txt
curl https://mtgdecks.net/decks/csv/Commander > ../../data/synergies/Commander.txt
curl https://mtgdecks.net/decks/csv/Legacy > ../../data/synergies/Legacy.txt
curl https://mtgdecks.net/decks/csv/Pauper > ../../data/synergies/Pauper.txt
curl https://mtgdecks.net/decks/csv/Vintage > ../../data/synergies/Vintage.txt
curl https://mtgdecks.net/decks/csv/Frontier > ../../data/synergies/Frontier.txt
curl https://mtgdecks.net/decks/csv/Peasant > ../../data/synergies/Peasant.txt
curl https://mtgdecks.net/decks/csv/Highlander > ../../data/synergies/Highlander.txt

# analyse decklists
pip install -r requirements.txt
./install_punkt.py
./build_model.py ../../data/AllCards.json ../../data/synergies
./tag_cards.py ../../data/AllCards.json ../../data/synergies > ../../data/synergies/card_tags.json
./card_synergies.py ../../data/AllCards.json ../../data/synergies/card_tags.json > ../../data/synergies/card_synergies.json
./analyze_decks.py ../../data/synergies/Standard.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_standard.json
./analyze_decks.py ../../data/synergies/Modern.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_modern.json
./analyze_decks.py ../../data/synergies/Commander.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_commander.json
./analyze_decks.py ../../data/synergies/Legacy.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_legacy.json
./analyze_decks.py ../../data/synergies/Pauper.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_pauper.json
./analyze_decks.py ../../data/synergies/Vintage.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_vintage.json
./analyze_decks.py ../../data/synergies/Frontier.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_frontier.json
./analyze_decks.py ../../data/synergies/Peasant.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_peasant.json
./analyze_decks.py ../../data/synergies/Highlander.txt ../../data/synergies/card_tags.json > ../../data/synergies/conditional_probabilities_highlander.json