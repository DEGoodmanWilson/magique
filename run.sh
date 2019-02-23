#!/usr/bin/env bash

# I really feel like there is a better way to do this. Maybe a Makefile. Maybe

pushd data_builders

# First, we need to grab the entire dataset from MTGJson
pushd downloadable
echo "Downloading basic card data..."
./run.sh
echo "...Done"
popd

# Now let's grab top decks for each format for analysis, then analyse them for synergies!
pushd synergies
echo "Finding synergies..."
./run.sh
echo "...Done"
popd

# Let's grab data from EDHRC (Warning, will take a long time!)
pushd edhrec
echo "Downloading data from EDHREC **WARNING WILL TAKE A WHILE**..."
./run.sh
echo "...Done"
popd

popd