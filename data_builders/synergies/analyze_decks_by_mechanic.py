#!/usr/bin/env python3

import re
import json
from pprint import pprint
import itertools
import sys

# This file looks at each list of decks, and extracts a list of mechanics present in each deck. Then it computes
# the conditional probmechanic, given that mechanic B appears in a deck, that mechanic A also appears in a deck.

if len(sys.argv) != 3:
    print("Usage: {0} <path-to-datafiles> <format>".format(sys.argv[0]))
    exit(1)

datapath = sys.argv[1]
format_name = sys.argv[2]

decks = []

mechanics_seen = []
# P(mechanic1 | mechanic2) = P(mechanic1 & mechanic2) / P(mechanic2)
num_decks_mechanic_seen_in = {} # the number of total decks a mechanic was seen in. Let's us calculate P(mechanic)
num_decks_combo_seen_in = {} # The number of total decks that the combination of mechanic1 and mechanic2 were seen in together. p(mechanic1 & mechanic2)



with open("{0}/synergies/{1}.csv".format(datapath, format_name.lower(), 'r')) as f:
    decks = f.readlines()

with open("{0}/synergies/card_mechanics.json".format(datapath)) as f:
    card_mechanics = json.load(f)

with open("{0}/normalized_card_names.json".format(datapath)) as f:
    normalized_card_names = json.load(f)


tagged_decks = []
for deck in decks:
    # Process this non-format into something we can use. Each deck looks like:
    #"2 mechanic of Death 3 Scrape 2 Another mechanic 1 Singleton\n"
    # _with_ the double quotes, ugh
    deck = deck.replace('"', '') # remove double quotes
    deck = deck.replace('\n', '') # remove the newline

    deck = re.split("\s?[\d]+\s", deck)[1:] # parse deck out into "mechanic of Death", "Scrape", "Another mechanic",...

    # Let's turn this into a list of normalized mechanic names.
    deck_mechanics = []
    for card_name in deck:
        card_name = normalized_card_names[card_name]

        # Find this card's mechanics, and log them
        if card_name in card_mechanics:
            for mechanic in card_mechanics[card_name]:
                deck_mechanics.append(mechanic)
                if mechanic not in mechanics_seen:
                    mechanics_seen.append(mechanic)

    tagged_decks.append(deck_mechanics)


# now, start over with decks of normalized mechanic names
decks = tagged_decks
for deck in decks:
  for mechanic1 in deck:
      # We have now seen mechanic_name in a deck. Increment the counter for this
      if mechanic1 in num_decks_mechanic_seen_in.keys():
          num_decks_mechanic_seen_in[mechanic1] += 1
      else:
          num_decks_mechanic_seen_in[mechanic1] = 1

      for mechanic2 in deck:
        # We have now seen both mechanic2 and mechanic1 in this deck, increment the counter for this as well
          if mechanic1 == mechanic2:
              continue

          if mechanic1 not in num_decks_combo_seen_in.keys():
              num_decks_combo_seen_in[mechanic1] = {}

          if mechanic2 not in num_decks_combo_seen_in[mechanic1]:
              num_decks_combo_seen_in[mechanic1][mechanic2] = 1
          else:
              num_decks_combo_seen_in[mechanic1][mechanic2] += 1

# now let's compute the conditional probmechanics

conditional_probmechanics = {}
num_decks = float(len(decks))
for mechanic1 in mechanics_seen: # TODO get that number from somewhere instead of hard coding it!
    for mechanic2 in mechanics_seen:
        #computer p(mechanic1 | mechanic2) = p(mechanic1 & mechanic2) / p(mechanic2)
        p_mechanic2 = num_decks_mechanic_seen_in[mechanic2] / num_decks

        if mechanic2 in num_decks_combo_seen_in[mechanic1]:
            p_mechanic1_mechanic2 = num_decks_combo_seen_in[mechanic1][mechanic2] / num_decks
        else:
            p_mechanic1_mechanic2 = 0.0

        if mechanic1 not in conditional_probmechanics.keys():
          conditional_probmechanics[mechanic1] = {}

        conditional_probmechanic = p_mechanic1_mechanic2 / p_mechanic2
        if conditional_probmechanic > 0:
            conditional_probmechanics[mechanic1][mechanic2] = conditional_probmechanic

print(json.dumps(conditional_probmechanics, indent=4))

