#!/usr/bin/env python3

import os
import sys
import json

def main():
  filepath = sys.argv[1]

  if not os.path.isfile(filepath):
    print("File path {} does not exist. Exiting...".format(filepath))
    sys.exit(1)

  with open(filepath) as f:
    cards = json.load(f)

  lowest_rank = 0
  highest_price = 0.0
  most_decks = 0
  for name, card in cards.items():
    if ('edhrec_rank' in card.keys()) and (card['edhrec_rank'] is not None) and (card['edhrec_rank'] > lowest_rank):
      lowest_rank = card['edhrec_rank']
    if ('price' in card.keys()) and (card['price'] is not None) and (card['price'] > highest_price):
      highest_price = card['price']
    if ('deck_count' in card.keys()) and (card['deck_count'] is not None) and (card['deck_count'] > most_decks):
      most_decks = card['deck_count']

  print(json.dumps({'lowest_rank': lowest_rank, 'highest_price': highest_price, 'most_decks': most_decks}, indent=4))

if __name__ == '__main__':  
  main()