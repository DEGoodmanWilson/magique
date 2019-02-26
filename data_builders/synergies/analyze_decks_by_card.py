#!/usr/bin/env python3

import re
import json
from pprint import pprint
import itertools
import sys

# This file looks at each list of decks, and extracts a list of cards present in each deck. Then it computes
# the conditional probability, given that card B appears in a deck, that card A also appears in a deck.

if len(sys.argv) != 3:
    print("Usage: {0} <path-to-datafiles> <format>".format(sys.argv[0]))
    exit(1)

datapath = sys.argv[1]
format_name = sys.argv[2]

decks = []

cards_seen = []
# P(card1 | card2) = P(card1 & card2) / P(card2)
num_decks_card_seen_in = {} # the number of total decks a card was seen in. Let's us calculate P(card)
num_decks_combo_seen_in = {} # The number of total decks that the combination of card1 and card2 were seen in together. p(card1 & card2)



with open("{0}/synergies/{1}.csv".format(datapath, format_name.lower(), 'r')) as f:
    decks = f.readlines()

with open("{0}/normalized_card_names.json".format(datapath)) as f:
    normalized_card_names = json.load(f)


normalized_decks = []
for deck in decks:
    # Process this non-format into something we can use. Each deck looks like:
    #"2 Card of Death 3 Scrape 2 Another Card 1 Singleton\n"
    # _with_ the double quotes, ugh
    deck = deck.replace('"', '') # remove double quotes
    deck = deck.replace('\n', '') # remove the newline

    deck = re.split("\s?[\d]+\s", deck)[1:] # parse deck out into "Card of Death", "Scrape", "Another Card",...

    # Let's turn this into a list of normalized card names.
    normalized_deck = []
    for card_name in deck:
        card_name = normalized_card_names[card_name]

        # mark that we have seen this card
        if card_name not in cards_seen:
          cards_seen.append(card_name)

        if card_name not in normalized_deck:
          normalized_deck.append(card_name)

    normalized_decks.append(normalized_deck)

# now, start over with decks of normalized card names
decks = normalized_decks
for deck in decks:
  for card1 in deck:
      # We have now seen card_name in a deck. Increment the counter for this
      if card1 in num_decks_card_seen_in.keys():
          num_decks_card_seen_in[card1] += 1
      else:
          num_decks_card_seen_in[card1] = 1

      for card2 in deck:
        # We have now seen both card2 and card1 in this deck, increment the counter for this as well
          if card1 == card2:
              continue

          if card1 not in num_decks_combo_seen_in.keys():
              num_decks_combo_seen_in[card1] = {}

          if card2 not in num_decks_combo_seen_in[card1]:
              num_decks_combo_seen_in[card1][card2] = 1
          else:
              num_decks_combo_seen_in[card1][card2] += 1

# now let's compute the conditional probabilities

conditional_probabilities = {}
num_decks = float(len(decks))
for card1 in cards_seen: # TODO get that number from somewhere instead of hard coding it!
    for card2 in cards_seen:
        #computer p(card1 | card2) = p(card1 & card2) / p(card2)
        p_card2 = num_decks_card_seen_in[card2] / num_decks

        if card2 in num_decks_combo_seen_in[card1]:
            p_card1_card2 = num_decks_combo_seen_in[card1][card2] / num_decks
        else:
            p_card1_card2 = 0.0

        if card1 not in conditional_probabilities.keys():
          conditional_probabilities[card1] = {}

        conditional_probability = p_card1_card2 / p_card2
        if conditional_probability > 0:
            conditional_probabilities[card1][card2] = conditional_probability

print(json.dumps(conditional_probabilities, indent=4))

