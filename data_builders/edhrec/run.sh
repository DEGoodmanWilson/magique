#!/usr/bin/env bash

pip install -r requirements.txt
./edhrec.py ../../data/AllCards.json > ../../data/edhrec/edhrec.json
./find_extremes.py ../../data/edhrec/edhrec.json > ../../data/edhrec/edhrec_extremes.json
