#!/usr/bin/env python

import json
import re

with open('../data/AllCards-x.json') as json_data:
    cards = json.load(json_data)

all_card_affinities = {}


with open('../data/types.json') as json_types:
    types = json.load(json_types)


# TODO this does work well with certain token creation cards :(
# Sarpadian Empires, Vol. VII
# Also "Angrath's Fury", ugh.

token_re = re.compile(r"([A-Z]\w* )+(artifact )?creature token")

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
        card_affinities = []

        # remove any reference to the cards name, like "Aspect of Mongoose" or "Fire Dragon"
        text = card['text'].replace(card['name'], '')

        text = re.sub(token_re, 'creature token', text)

        # check for references to creature types.
        for i in range(len(types)):
            if(re.search(type_res[i], text)):
                card_affinities.append(types[i])
        if (len(card_affinities) > 0):
            all_card_affinities[name] = card_affinities

print(json.dumps(all_card_affinities, indent=4, separators=(',', ': ')))
