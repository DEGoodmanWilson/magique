#!/usr/bin/env python

import gensim
from gensim import corpora
from six import iteritems
import json
from pprint import pprint
import massage_text

model = gensim.models.ldamodel.LdaModel.load("cards.lda")
dictionary = corpora.Dictionary.load("cards.dict")



with open('../data/AllCards-x.json') as json_data:
  cards = json.load(json_data)

all_card_tags = {}

# We want to do a couple of things for each card.
# 1) we want to break apart the oracle text into sentences: Each sentence is a "document"
# 2) we want to transform each document into a bag of words

for name, card in cards.iteritems():
  # pprint(card)

    # skip lands and tokens!
    if("token" == card['layout']):
        continue
    if("Land" in card['types']):
        continue

    if 'text' in card:
        card_tags = []
        sentences = massage_text.massage(card['text'], card['name'])
        for sentence in sentences:
            # classify the sentence
            vec = dictionary.doc2bow(sentence)
            tag_probs = model[vec]
            # We're going to be _very_ naive here, and just take the one tag that is most likely, and run with that.
            if(len(tag_probs) > 0):
                tag = sorted(tag_probs, key=lambda x: x[1])[-1][0]
                card_tags.append(tag)
        if(len(card_tags) > 0):
            all_card_tags[name] = card_tags

print(json.dumps(all_card_tags, indent=4, separators=(',', ': ')))
