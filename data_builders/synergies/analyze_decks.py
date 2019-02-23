#!/usr/bin/env python3

import re
import json
from pprint import pprint
import itertools
import sys

if len(sys.argv) != 3:
    print "Usage: {0} <deck-datafile> <tag-datafile>".format(sys.argv[0])
    exit(1)

datafile = sys.argv[1]
tagfile = sys.argv[2]

decks = []
all_appearances = 0
mechanic_appearances = [0] * 250
mechanic_combo_appearances = [[0 for i in range(250)] for j in range(250)] # keys are mechanic name. I'd prefer a sparse matric, but you know...

with open(datafile, 'r') as f:
    decks = f.readlines()

with open(tagfile) as f:
    card_tags = json.load(f)

for deck in decks:
    # Process this non-format into something we can use. Each deck looks like:
    #"2 Card of Death 3 Scrape 2 Another Card 1 Singleton\n"
    # _with_ the double quotes, ugh
    deck = deck.replace('"', '') # remove double quotes
    deck = deck.replace('\n', '') # remove the newline

    deck = re.split("\s?\d\s", deck)[1:] # parse deck out into "Card of Death", "Scrape", "Another Card",...

    # deck = re.split("\s?(\d)\s", deck)[1:] # parse deck out into "2", "Card of Death", "3", "Scrape", ...
    # deck = zip(deck[0::2], deck[1::2])
    # we now have a deck that looks like
    # [ ("1", "Abrade"), ("4", "Cancel"), ...]

    # Let's turn this into a list of cards with quantities, then a list of abilities.
    # deck_mechanics = []
    # for card in deck:
    #     name = card[1]
    #     count = int(card[0])
    #     for i in range(count):
    #         card_tagged = name in card_tags
    #         if(card_tagged):
    #             for tag in card_tags[name]:
    #                 deck_mechanics.append(tag)

    # Let's turn this into a list of abilities.
    deck_mechanics = []
    for card_name in deck:
        card_tagged = card_name in card_tags
        if(card_tagged):
            for tag in card_tags[card_name]:
                deck_mechanics.append(tag)
    
    deck_mechanics.sort()
    deck_mechanics_set = set(deck_mechanics) # Get a list of unique mechanics
    for mechanic in deck_mechanics_set:
        mechanic_appearances[mechanic] += 1

    for mechanic1 in deck_mechanics_set:
        # First, let's identify when two distinct mechanics co-occur
        for mechanic2 in deck_mechanics_set:
            if mechanic1 != mechanic2:
                mechanic_combo_appearances[mechanic1][mechanic2] += 1
        # Now let's identify if the same mechanic co-occurs on different cards
        if deck_mechanics.count(mechanic1) > 1:
            mechanic_combo_appearances[mechanic1][mechanic1] += 1


conditional_probabilities = {}
for mechanic1 in range(250): # TODO get that number from somewhere instead of hard coding it!
    for mechanic2 in range(250):
        #computer p(m1 | m2)
        combo_idx = "{0}|{1}".format(mechanic1,mechanic2)
        if mechanic_appearances[mechanic2] > 0:
            prob = float(mechanic_combo_appearances[mechanic1][mechanic2]) / mechanic_appearances[mechanic2]
            if(prob > 0):
                conditional_probabilities[combo_idx] = prob

# pprint(conditional_probabilities)
print(json.dumps(conditional_probabilities, indent=4, separators=(',', ': ')))

