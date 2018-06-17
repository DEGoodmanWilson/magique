import re
import json
from pprint import pprint
import itertools

decks = []
all_appearances = 0
mechanic_appearances = [0] * 250
mechanic_combo_appearances = [[0 for i in range(250)] for j in range(250)] # keys are mechanic name. I'd prefer a sparse matric, but you know...

with open('../data/Modern.txt', 'r') as f:
    decks = f.readlines()

with open('../data/card_tags.json') as f:
    card_tags = json.load(f)

for deck in decks:
    deck = deck.replace('"', '')
    deck = deck.replace('\n', '')
    deck = re.split("\s?(\d)\s", deck)[1:]
    deck = zip(deck[0::2], deck[1::2])
    # we now have a deck that looks like
    # [ ("1", "Abrade"), ("4", "Cancel"), ...]
    # Let's turn this into a list of cards, then a list of abilities.
    deck_mechanics = []
    for card in deck:
        name = card[1]
        count = int(card[0])
        for i in range(count):
            card_tagged = name in card_tags
            if(card_tagged):
                for tag in card_tags[name]:
                    deck_mechanics.append(tag)
    
    deck_mechanics.sort()
    deck_mechanics_set = set(deck_mechanics) # Get a list of unique mechanics
    for mechanic in deck_mechanics_set:
        mechanic_appearances[mechanic] += 1

    for mechanic1 in deck_mechanics_set:
        for mechanic2 in deck_mechanics: # We are not iterating over unique items here. This lets mechanics that appear more frequently be more represented
            mechanic_combo_appearances[mechanic1][mechanic2] += 1


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

