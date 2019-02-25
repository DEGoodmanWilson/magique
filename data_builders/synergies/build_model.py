#!/usr/bin/env python3

import sys
import gensim
from gensim import corpora
from six import iteritems
import json
from pprint import pprint
import massage_text
from collections import defaultdict

# https://rstudio-pubs-static.s3.amazonaws.com/79360_850b2a69980c4488b1db95987a24867a.html

datapath = sys.argv[1]

with open(datapath+'/AllCards.json') as json_data:
  cards = json.load(json_data)

card_sentences = []

with open(datapath+'/normalized_card_names.json') as normalized_json:
  normalized_card_names = json.load(normalized_json)

# We want to do a couple of things for each card.
# 1) we want to break apart the oracle text into sentences: Each sentence is a "document"
# 2) we want to transform each document into a bag of words

for name, card in cards.items():
  # skip lands and tokens!
  if("token" == card['layout']):
    continue
  if("Land" in card['types']):
    continue

  if 'text' in card:
    text = massage_text.massage(card['text'], card['name'])
    card_sentences += text

# remove words that appear only once
frequency = defaultdict(int)
for text in card_sentences:
    for token in text:
        frequency[token] += 1
card_sentences = [[token for token in text if frequency[token] > 1]
         for text in card_sentences]

# Construct the bags of words
dictionary = corpora.Dictionary(card_sentences)
dictionary.save(datapath+"/synergies/cards.dict")

corpus = [dictionary.doc2bow(text) for text in card_sentences]

# build the LDA model
num_topics = 150
ldamodel = gensim.models.ldamodel.LdaModel(corpus, num_topics=num_topics, id2word=dictionary, passes=1)

pprint(ldamodel.print_topics(num_topics=num_topics, num_words=5))

ldamodel.save(datapath+"/synergies/cards.lda")

# ldamodel.get_document_topics(dictionary.doc2bow(['']) )