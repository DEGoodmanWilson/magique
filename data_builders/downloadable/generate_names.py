#!/usr/bin/env python3

import sys  
import os
import json

# https://stackabuse.com/read-a-file-line-by-line-in-python/

def generate_names():  
  filepath = sys.argv[1]
  outpath = sys.argv[2]

  if not os.path.isfile(filepath):
    print("File path {} does not exist. Exiting...".format(filepath))
    sys.exit()
  if not os.path.isdir(outpath):
    print("Output path {} does not exist. Existin...".format(outpath))

  canonical_card_names = []
  non_canonical_card_names = {}

  with open(filepath, 'r') as f:
    catalog = json.load(f)

  # now, let's create a list of all the normalized names.
  # Then we we'll create a map to figure out different ways they could have been munged
  for name, card in catalog.items():
    # begin by assuming the name we have is the canonical name. Because in most
    # cases it will be
    canonical_name = name

    # But maye we have a split card?
    if ("names" in card.keys()) and (len(card["names"]) > 1) and (card['layout'] != 'meld'):
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
      non_canonical_card_names[canonical_name.replace(" // ", "/")] = canonical_name
      non_canonical_card_names[canonical_name.replace(" // ", "/").lower()] = canonical_name
    # At least two cards have quotes around their names:
    # "Ach! Hans, Run!" and "Rumors of My Death . . ."
    # But not everyone includes those quotes
    if '"' in canonical_name:
      non_canonical_card_names[canonical_name.replace('"', '')] = canonical_name
      non_canonical_card_names[canonical_name.replace('"', '').lower()] = canonical_name      
    # Sometimes, mtgdecks gives us card names like
    # "Circle of Protection : Green (Circle of Prote" (notice the extra space before the colon)
    # WTF
    if "Circle of Protection:" in canonical_name:
      rem_len = 42 - len(canonical_name)
      new_name = (canonical_name+" ("+canonical_name[0:rem_len]).replace(": ", " : ")
      non_canonical_card_names[new_name] = canonical_name

  with open(outpath+'/canonical_card_names.json', 'w') as outfile:  
    json.dump(canonical_card_names, outfile, indent=4)
  with open(outpath+'/normalized_card_names.json', 'w') as outfile:  
    json.dump(non_canonical_card_names, outfile, indent=4)

if __name__ == '__main__':  
   generate_names()