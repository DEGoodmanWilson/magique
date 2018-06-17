#!/usr/bin/env python

import gensim
from gensim import corpora
from six import iteritems
import json
from pprint import pprint
import massage_text

# https://rstudio-pubs-static.s3.amazonaws.com/79360_850b2a69980c4488b1db95987a24867a.html


with open('../data/AllCards-x.json') as json_data:
    cards = json.load(json_data)

card_sentences = []

# We want to do a couple of things for each card.
# 1) we want to break apart the oracle text into sentences: Each sentence is a "document"
# 2) we want to transform each document into a bag of words

for name, card in cards.iteritems():

    # skip lands and tokens!
    if("token" == card['layout']):
        continue
    if("Land" in card['types']):
        continue

    if 'text' in card:
        text = massage_text.massage(card['text'], card['name'])
    card_sentences = card_sentences + text

# Construct the bags of words
dictionary = corpora.Dictionary(card_sentences)
dictionary.save("cards.dict")
corpus = [dictionary.doc2bow(text) for text in card_sentences]

# build the LDA model
num_topics = 250
ldamodel = gensim.models.ldamodel.LdaModel(corpus, num_topics=num_topics, id2word = dictionary, passes=20)

pprint(ldamodel.print_topics(num_topics=num_topics, num_words=5))

ldamodel.save("cards.lda")

ldamodel.get_document_topics(dictionary.doc2bow(['']) )