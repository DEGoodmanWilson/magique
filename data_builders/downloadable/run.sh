#!/bin/bash

curl https://mtgjson.com/json/AllCards.json > ../../data/AllCards.json
./generate_names.py ../../data/AllCards.json ../../data
