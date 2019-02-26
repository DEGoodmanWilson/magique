#!/usr/bin/env python3

from bs4 import BeautifulSoup
import requests
import json
import sys
import time
import unidecode


def edhrec_url_normalize(name):
  return unidecode.unidecode(name.lower().replace(" // ", "-").replace("'", "").replace(",", "").replace(" ", "-"))

def generate_edhrec_url(name):
#  https://edhrec.com/cards/kodamas-reach
  return "https://edhrec.com/cards/"+edhrec_url_normalize(name)

def evaluate_card(card_name):
  done = False
  backoff = 1
  while not done:
    try:
      response = requests.get(generate_edhrec_url(card_name))
      done = True
    except:
      if backoff > 100:
        raise
      time.sleep(backoff)
      backoff = backoff * 2
  if response.status_code == 404:
    return 0, []

  soup = BeautifulSoup(response.content, features="html.parser")

  # Find the <script> tag that has the data as a JSON dict
  script_tags = soup.find_all('script')

  data = None
  our_data = {}

  for script in script_tags:
    if script.string and ('const json_dict' in script.string):
      json_data = script.string.split('const json_dict = ')[-1].replace(';', '')
      data = json.loads(json_data)
      break

  if data is None:
    return None

  # Here's where we parse the stuff we want out of the very tasty JSON that EDHREC provides
  if ('card' in data.keys()) and ('cards' in data['card']) and ('edhrec_rank' in data['card']['cards'][0]['dict'].keys()):
    our_data['edhrec_rank'] = data['card']['cards'][0]['dict']['edhrec_rank']
  else:
    our_data['edhrec_rank'] = None

  if ('card' in data.keys()) and ('label' in data['card']):
    our_data['deck_count'] = int(data['card']['label'].split()[0])
  else:
    our_data['deck_count'] = 0

  our_data['price'] = 0.0
  divisor = 0
  if 'card' in data.keys():
    if data['card']['tcgplayer']:
    	our_data['price'] += float(data['card']['tcgplayer']['price'])
    	divisor += 1
    if data['card']['cardhoarder']:
    	our_data['price'] += float(data['card']['cardhoarder']['price'])
    	divisor += 1
    if data['card']['cardkingdom']:
    	our_data['price'] += float(data['card']['cardkingdom']['price'])
    	divisor += 1
    if divisor > 0:
      our_data['price'] = our_data['price'] / divisor

  if ('card' in data.keys()) and ('tribe' in data['card'].keys()):
    our_data['tribe'] = data['card']['tribe']

  our_data['synergies'] = {}
  if data['cardlists']:
    for cardlist in data['cardlists']:
      if cardlist['tag'] is not 'topcommanders':
        for card in cardlist['cardviews']:
          syn_name = card['name']
          if card_name == syn_name:
            continue
          synergy = float(card['label'].split('<br />')[-1].split()[0].strip('%'))/100
          our_data['synergies'][syn_name] = synergy

  return our_data


datafile = sys.argv[1] # TODO CHECK THIS

with open(datafile) as fd:
  data = json.load(fd)
card_names = data.keys()

# TODO Load card names so we can pick up where we left off, if need be.
seen = []
cards = {}

# Iterate over all the cards
for card_name in card_names:
  alljson_card_name = card_name
  #ignore backs of double-faced cards, and derive the canonical name for split cards
  if 'layout' in data[card_name].keys():
    # ignore back side of double-faced cards
    if (data[card_name]['layout'] == 'transform') or (data[card_name]['layout'] == 'flip'):
      if card_name != data[card_name]['names'][0]:
        continue

    # skip meld cards
    if data[card_name]['layout'] == 'meld':
      continue;

    # deal with split cards
    if data[card_name]['layout'] == 'split':
      card_name = " // ".join(data[card_name]['names'])
      # url_card_name = data[card_name]['names'][0]


  # don't process a card we've already processed
  if card_name in seen:
    continue

  # only look at commander-legal cards
  legal = False

  if 'legalities' in data[alljson_card_name].keys():
    legalities = data[alljson_card_name]['legalities']
    if 'commander' in legalities:
      if legalities['commander'] == "Legal":
        legal = True

  if not legal:
    continue

  card_data = evaluate_card(card_name)
  if alljson_card_name != card_name:
    card_data['alljson_name'] = alljson_card_name

  seen.append(card_name)
  cards[card_name] = card_data

print(json.dumps(cards, indent=4))
