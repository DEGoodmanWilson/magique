#!/usr/env python

from __future__ import print_function

from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.model_selection import train_test_split
from sklearn import metrics
from sklearn.preprocessing import MultiLabelBinarizer
from sklearn.multiclass import OneVsRestClassifier
from sklearn.svm import SVC
import json

# load training data

json_data = open("training.json").read()
total_data = json.loads(json_data)

x = total_data["text"]
y = total_data["abilities"]

# let's just do flying for now
for item in y:


mlb = MultiLabelBinarizer()
y_enc = mlb.fit_transform(y)

data_train, data_test, Y_train, Y_test = train_test_split(x, y_enc, test_size=0.25)

# vectorize bigrams
print("Extracting features from the training data using a sparse vectorizer")

vectorizer = TfidfVectorizer(ngram_range=(1, 2), token_pattern=r'\b\w+\b', min_df=1)

X_train = vectorizer.fit_transform(data_train)
X_test = vectorizer.transform(data_test)

clf = OneVsRestClassifier(SVC(probability=True))
clf.fit(X_train, Y_train)
predictions = clf.predict(X_test)
print(predictions)

my_metrics = metrics.classification_report(Y_test, predictions)
print(my_metrics)
