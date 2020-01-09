#!/usr/bin/env bash

pip3 install -r requirements.txt
./edhrec.py ../../data > ../../data/edhrec/edhrec.json
./find_extremes.py ../../data/edhrec/edhrec.json > ../../data/edhrec/edhrec_extremes.json
