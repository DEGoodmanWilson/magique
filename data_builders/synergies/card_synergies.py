#!/usr/bin/env python3

import json
import re

datafile = sys.argv[1]
tagfile = sys.argv[2]

with open(datafile) as json_data:
    cards = json.load(json_data)

all_card_synergies_and_subtypes = {}

with open(tagfile) as json_types:
    types = json.load(json_types)


# TODO this does work well with certain token creation cards :(
# Sarpadian Empires, Vol. VII
# Also "Angrath's Fury", ugh.

token_re = re.compile(r"((?:[A-Z]\w* )+)(?:artifact )?creature token")

type_res = []
for type in types:
    type_res.append(re.compile(r"\b{0}\b".format(type))) # do not ignore case!

for name, card in cards.iteritems():
  # pprint(card)

    # skip lands and tokens!
    if("token" == card['layout']):
        continue
    if("Land" in card['types']):
        continue

    if 'text' in card:
        card_annotations = {}
        # remove any reference to the cards name, like "Aspect of Mongoose" or "Fire Dragon"
        text = card['text'].replace(card['name'], '')

        # remove any reference to token creation; creating a token does not indiciate an synergy for that card type
        no_token_text = re.sub(token_re, 'creature token', text)

        # check for references to creature types to find creature types this card has an synergy for.
        card_synergies = []
        for i in range(len(types)):
            if(re.search(type_res[i], no_token_text)):
                card_synergies.append(types[i])
        if (len(card_synergies) > 0):
            card_annotations['synergies'] = card_synergies

        # Now, if this is a creature card, and it creates a token, add that token type to this creatures subtypes
        card_additional_subtypes = []
        token_matches = re.search(token_re, text)
        if(token_matches != None):
            new_subtypes = token_matches.group(1)
            card_annotations['sub_types'] = new_subtypes.split()

        # Add them into the global object
        if(card_annotations):
            all_card_synergies_and_subtypes[name] = card_annotations

print(json.dumps(all_card_synergies_and_subtypes, indent=4, separators=(',', ': ')))
