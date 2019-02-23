#!/usr/bin/env python

import sys  
import os
import json
import csv

# https://stackabuse.com/read-a-file-line-by-line-in-python/

def generate_names():  
  canonical_card_names = []
  non_canonical_card_names = {}

  with open('AllCards.json') as f:
    catalog = json.load(f)

  # now, let's create a list of all the normalized names.
  # Then we we'll create a map to figure out different ways they could have been munged
  for name, card in catalog.items():
    # begin by assuming the name we have is the canonical name. Because in most
    # cases it will be
    canonical_name = name

    # But maye we have a split card?
    if ("names" in card.keys()) and (len(card["names"]) > 1):
      canonical_name = " // ".join(card["names"])

    if canonical_name not in canonical_card_names:
      canonical_card_names.append(canonical_name)

    non_canonical_card_names[canonical_name] = canonical_name #Yes, I did this on purpose
    non_canonical_card_names[canonical_name.lower()] = canonical_name #Yes, I did this on purpose
    if name != canonical_name:
      non_canonical_card_names[name] = canonical_name
    non_canonical_card_names[name.lower()] = canonical_name
    if "//" in canonical_name:
      non_canonical_card_names[canonical_name.replace(" // ", "_")] = canonical_name
      non_canonical_card_names[canonical_name.replace(" // ", "_").lower()] = canonical_name

  with open('canonical_card_names.json', 'w') as outfile:  
    json.dump(canonical_card_names, outfile, indent=4)
  with open('normalized_card_names.json', 'w') as outfile:  
    json.dump(non_canonical_card_names, outfile, indent=4)

if __name__ == '__main__':  
   generate_names()