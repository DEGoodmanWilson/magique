# Extracting card features from rules text

The challenge: Can we identify and extract interesting card features from the rules text? The answer is _yes_, and this code does just that.

What makes Magic cards interesting is the abilities and powers that the rules text on the card grants it. Moreover, although every card is different, we can probably find interesting ways of lumping cards together based on similarities in their rules text. Let's call those similarities _mechanics_.

For example, here is a simple mechanic we often find printed on many cards:

> Flying

Or consider the following examples of rules texts:

> Draw a card.

> Whenever [card name] enters the battlefield, draw a card.

> {t}: Draw a card.

> Whenever equipped creature deals combat damage to a player, you may draw a card.

We might classify these together under a mechanic called "card drawing".

The goal of the code in this folder is to look at each and every card ever printed, and come up with _n_ mechanics, by finding similarities among rules texts and lumping them together.

To do this, we look at each card, and breakt the rules text up into sentences. We then apply a technique called [_latent dirichlet allocation_](https://en.wikipedia.org/wiki/Latent_Dirichlet_allocation) to lump sentences together based on the words they contain. All this is done in the file `build_model.py`. Since this is all being done automatically, each mechanic is labeled with a number, rather than a descriptive name. So the mechanics are sometimes opaque. But if you look at the output, you might recoginize some of them by looking at the key words associated with eac mechanic!

Once we have our _n_ mechanics, we then go back over every card ever printed, pull it apart into sentences _again_, and classify each sentence using the model built in the previous step. Thus, we might discover, for example, that ["Solemn Simulacrum"](https://scryfall.com/card/ddu/62) is tagged as having mechanics `3` and `210`. (When we look at the card, we might naturally think of it as having "land tutor" and "card drawing", but don't let the labels bother you.). `tag_cards.py` is the file that performs this tagging, and it outputs JSON that is used by the genetic algorithm in the containing folder for building decks.


# We're not done!

The next thing to be accomplished is to pull down a list of winning decks in a variety of formats, and start computing the conditional probabilities over card pairs A and B: What is the probability that a deck that contains a card with a mechanism that A has _also_ contains a card with a mechanism that B has? These conditional probabilities can be used by the genetic algorithm in the containing folder to understand how power any given card combination is likley to be, based on past tournament experience.