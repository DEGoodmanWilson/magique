# Magique
## A program for building interesting Magic: The Gathering decks using old-school genetic algorithms

This code is in very early stages. It's a mess. You'll see. Forgive me, as I just wanted to hack, but we'll get this code in order before too long.

## So you want to give it a go?

Building should be reasonably straightforward. Magique depends on CMake and Conan, and some special sauce:

Add vthiery's conan repo to the list of conan repos

```
conan remote add vthiery https://api.bintray.com/conan/vthiery/conan-packages
```

and do the conan install and build dance

```
conan install .
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build .
```

First, we need to gather all the data on all the Magic cards ever made. We're going to do that by grabbing a JSON file from [MTG JSON](https://mtgjson.com/) (Thanks, y'all!!)

```
curl https://mtgjson.com/json/AllCards.json.zip | tar -xf - -C data
```

Export your collection of Magic cards into a `csv` file in the `data` directory with the following format.
```
Quantity,Name,other thing,another thing
```

Be sure to leave the first line of the CSV file as the header, because the program will skip it. Look at `collection_dons.csv` for an example. This file was generated with "MTG Manager" on iOS. TODO: I'd like to be more flexible about how the collections file is stored, to support other services!

Now, because this code is a less of pile of flaming shit than it used to be, most options you want to set can be entered into the command line. Just run the binary to get some help text

``` bash
./bin/magique
```

Once you've got some ideas for your deck in mind (key cards to include, colors to focus on), run 
```bach
bin/magique path/to/your/collection.csv
```
and wait. It'll take a while to read in all 34,000 magic cards into memory, and then even more time to run through the one million deck combinations that by default it will try out in the course of the generic algorithm. At the end, you'll get a JSON array of all the selected cards, along with some arcane metrics describing the fitness of the deck.

## What next?

There is so much work to do. So much.

* Continue fixes to GA2, and release as a standalone library
* Continue to decouple the various classes, which are too closely coupled for my comfort
* Continue transforming that ugly `main()` and turn it into a pretty CLI
* Optimizations! So many opportunities for optimization
* The fitness function is really ugly, and hard to customize. Mostly it needs to be streamlined. I bet we don't need the current level of complexity for describing card abilities and interactions. 
* Continue to build database of card abilities, affinities, and mechanic linearities. Apply some fancy NLP to help automate this. Right now only a portion of Kaladesh and Aether Revolt (and a tiny fraction of Amonkhet) have annotations to help identify card combos and linearites. And only a small number of card mechanics have been identified. And even then the system for representing them is not very good…

    It would be even better if we could build some ML tools to automatically tag cards, and card interactions!
* Refine system for representing card interactions and abilities in more interesting and nuanced ways
* Specify key mechanics to include in a deck
* Allow user to specify their own mana curves and card type proportions
* Unit tests, HELLO WHY HAVEN'T I WRITTEN ANY TESTS YET? Also integration tests. And functional tests, and testing tests and other tests and all the tests

## How can I help?

I'm so glad you asked. Just start digging in on anything you see that's missing that you can fix. Even just more data on how Magic cards work together would be awesome.

And if you just want to show your appreciation, you can always use my [Cardmarket affiliate link](https://cardmarket.com/?referrer=DEGoodmanWilson) when buying for yourself, or if you're feeling especially generous [Cardmarket coupons](https://www.cardmarket.com/en/Magic/MainPage/buyCoupons) are always welcome!

## Notes on abilities and interactions

The current system is quite broken. There is no way to distinguish "grants haste" from "grants haste until end of turn". There's no way to capture subtle things like "target create cannot block this turn". It would be nice to name an ability, and then assign attributes like "permanently", "temporarily", "only on itself", "grants to others", &c. Same with abilities that place or remove counters on targets. How should we represent "place a -1/-1 counter on target creature you own" from "remove a -1/-1 counter on target opponent creature", for example? Affinities at the moment can only disjunctions of types/subtypes; there is no way to indicate conjunctions (e.g., and especially "artifact creatures"). There's also no way to indicate strenght of ability, e.g. Fabricate 1 or Scry 2. There is no way to indicate if an ability is triggered, or its cost.

In other words, the system of identifying mechanics and abilities, and how they interact with other cards needs _significant_ refinement. Help wanted.

I _strongly_ suspect that the correct answer to this problem is to grossly _simplify_ the way this stuff is represented! I'm already at work on this problem, and will update this document when I have more details to share!
