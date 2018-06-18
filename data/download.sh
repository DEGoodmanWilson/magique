#!/bin/bash

curl https://mtgjson.com/json/AllCards-x.json.zip | tar -xf - -C .
curl https://mtgdecks.net/decks/csv/Standard > Standard.txt
curl https://mtgdecks.net/decks/csv/Modern > Modern.txt
curl https://mtgdecks.net/decks/csv/Commander > Commander.txt
curl https://mtgdecks.net/decks/csv/Legacy > Legacy.txt
curl https://mtgdecks.net/decks/csv/Pauper > Pauper.txt
curl https://mtgdecks.net/decks/csv/Vintage > Vintage.txt
curl https://mtgdecks.net/decks/csv/Frontier > Frontier.txt
curl https://mtgdecks.net/decks/csv/Peasant > Peasant.txt
curl https://mtgdecks.net/decks/csv/Highlander > Highlander.txt