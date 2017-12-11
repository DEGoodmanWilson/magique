# Magique
## A program for building interesting Magic: The Gathering decks using old-school genetic algorithms

This code is in very early stages. It's a mess. You'll see. Forgive me, as I just wanted to hack, but we'll get this code in order before too long.

## So you want to give it a go?

Building should be reasonably straightforward. Magique depends on CMake and Conan, and some special sauce:

Add DEGoodmanWilson's conan repo to the list of conan repos

```
conan remote add DEGoodmanWilson https://api.bintray.com/conan/degoodmanwilson/opensource
```

and do the usual conan install and build dance

```
conan install --build=missing
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build .
```

First, we need to gather all the data on all the Magic cards ever made. We're going to do that by scraping mtgapi.com. Build and run `magique_scraper` and pipe its output into a file called `data/catalog.json`.

```
./bin/magique_scraper > data/catalog.json
```

Export your collection of Magic cards into a `csv` file in the `data` directory with the following format
```
Quantity,Name,other thing,another thing
```

Be sure to leave the first line of the CSV file as the header, because the program will skip it. Look at `collection_dons.csv` for an example. This file was generated with "MTG Manager" on iOS.

Now, because this code is a pile of flaming shit, any options you want to set will need to be manually coded into `main.cpp`. You'll need to tell it what file contains your collection (or, hell, just use my collection to get started, and leave things as they are).

If you want to build a deck around a specific card, look at the line
```
deck::add_key_card(master_catalog.at("Electrostatic Pummeler"));
```
for inspiration. If not, leave that line out.

The run `bin/magique` and wait. It'll take a while to read in all 34,000 magic cards into memory, and then even more time to run through the one million deck combinations that by default it will try out in the course of the generic algorithm. At the end, you'll get a JSON array of all the selected cards, along with some arcane metrics describing the fitness of the deck.

## What next?

There is so much work to do. So much.

* Continue fixes to GA2, and release as a standalone library
* Continue to decouple the various classes, which are too closely coupled for my comfort
* That that ugly `main()` and turn it into a pretty CLI
* Optimizations! So many opportunities for optimization
* Continue to build database of card abilities, affinities, and mechanic linearities. Apply some fancy NLP to help automate this. RIght now only a portion of Kaladesh and Aether Revolt (and a tiny fraction of Amonkhet) have annotations to help identify card combos and linearites. And only a small number of card mechanics have been identified. And even then the system for representing them is not very good…
* Refine system for representing card interactions and abilities in more interesting and nuanced ways
* Specify key mechanics to include in a deck
* Allow user to specify their own mana curves and card type proportions
* Unit tests, HELLO WHY HAVEN'T I WRITTEN ANY TESTS YET? Also integration tests. And functional tests, and testing tests and other tests and all the tests

## How can I help?

I'm so glad you asked. Just start digging in on anything you see that's missing that you can fix. Even just more data on how Magic cards work together would be awesome.

And if you just want to show your appreciation, you can always send me a booster. Call it research ;) (Actively collecting French-language Kaladesh and Aether Revolt, but honestly cards from any expansion and in any and all languages heartily accepted!)

## Notes on abilities and interactions

The current system is quite broken. There is no way to distinguish "grants haste" from "grants haste until end of turn". There's no way to capture subtle things like "target create cannot block this turn". It would be nice to name an ability, and then assign attributes like "permanently", "temporarily", "only on itself", "grants to others", &c. Same with abilities that place or remove counters on targets. How should we represent "place a -1/-1 counter on target creature you own" from "remove a -1/-1 counter on target opponent creature", for example? Affinities at the moment can only disjunctions of types/subtypes; there is no way to indicate conjunctions (e.g., and especially "artifact creatures"). There's also no way to indicate strenght of ability, e.g. Fabricate 1 or Scry 2.

In other words, the system of identifying mechanics and abilities, and how they interact with other cards needs _significant_ refinement. Help wanted.