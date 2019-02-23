#!/usr/bin/env python

import sys  
import os
import json
import csv
import re

# https://stackabuse.com/read-a-file-line-by-line-in-python/

def main():  
  filepath = sys.argv[1]

  if not os.path.isfile(filepath):
    print("File path {} does not exist. Exiting...".format(filepath))
    sys.exit()

# TODO load canonical card names
  with open('normalized_card_names.json') as f:
    normalized_card_names = json.load(f)

  cards = {}
  with open(filepath) as fp:
    csv_reader = csv.reader(fp, delimiter=',')

    for fields in csv_reader:
      if fields[0] == "Quantity":
        continue

      qty = int(fields[0])
      name = fields[1]

      # NORMALIZE NAME
      # first strip off any versioning. Could look like:
      # moutain2
      # mountain (a)
      # https://regex101.com/r/JmW3rK/2
      version_regex = r"(\s\([a-zA-Z]\)|\d)$"
      name = re.sub(version_regex, "", name)

      #now find ther cononical name from the loaded table
      if(name not in normalized_card_names.keys()):
        print("Can't find "+name+" !")
        exit(1)
      canonical_name = normalized_card_names[name]

      if canonical_name in cards.keys():
        cards[canonical_name] += qty
      else:
        cards[canonical_name] = qty

  print(json.dumps(cards, sort_keys=True, indent=4))

if __name__ == '__main__':  
   main()
