#!/usr/env python

from sklearn.feature_extraction.text import TfidfVectorizer
import json

# load training data

json_data=open("training.json").read()
corpus = json.loads(json_data)


# vectorize bigrams
vectorizer = TfidfVectorizer(ngram_range=(1, 2), token_pattern=r'\b\w+\b', min_df=1)
vectorizer.fit_transform(corpus["text"])
analyze = vectorizer.build_analyzer()
print analyze(corpus["text"][6])