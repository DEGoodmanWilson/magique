# -*- coding: utf-8 -*-

import re
import nltk
from stop_words import get_stop_words
import nltk.data
from nltk import word_tokenize
import numpy as np
from nltk.stem.porter import PorterStemmer
from pprint import pprint

paren_regex = re.compile(r"\(.*?\)", re.IGNORECASE)
punc_regex = re.compile(r"[^\w\s{}\-\+/']", re.IGNORECASE)
neg_regex = re.compile(r"n't", re.IGNORECASE)
stop_regex = re.compile(r"\.", re.IGNORECASE)

nltk.download('punkt')
sentence_tokenizer = nltk.data.load('tokenizers/punkt/english.pickle')
# en_stop = get_stop_words('en')
en_stop = [
"a",
"about",
"against",
"am",
"an",
"and",
"are",
"aren't",
"as",
"at",
"be",
"because",
"been",
"being",
"between",
"but",
"by",
"can't",
"cannot",
"could",
"couldn't",
"did",
"didn't",
"do",
"does",
"doesn't",
"doing",
"don't",
"few",
"for",
"from",
"further",
"had",
"hadn't",
"hasn't",
"have",
"haven't",
"having",
"he",
"he'd",
"he'll",
"he's",
"her",
"here",
"here's",
"hers",
"herself",
"him",
"himself",
"his",
"how",
"how's",
"i",
"i'd",
"i'll",
"i'm",
"i've",
"in",
"into",
"is",
"isn't",
"it",
"it's",
"its",
"itself",
"let's",
"me",
"more",
"most",
"mustn't",
"my",
"myself",
"no",
"nor",
"of",
"off",
"on",
"or",
"other",
"ought",
"our",
"ours",
"ourselves",
"out",
"over",
"shan't",
"she",
"she'd",
"she'll",
"she's",
"should",
"shouldn't",
"so",
"some",
"such",
"than",
"that",
"that's",
"the",
"their",
"theirs",
"them",
"themselves",
"then",
"there",
"there's",
"these",
"they",
"they'd",
"they'll",
"they're",
"they've",
"this",
"those",
"through",
"to",
"too",
"under",
"very",
"was",
"wasn't",
"we",
"we'd",
"we'll",
"we're",
"we've",
"were",
"weren't",
"what",
"what's",
"when",
"when's",
"where",
"where's",
"which",
"while",
"who",
"who's",
"whom",
"why",
"why's",
"with",
"won't",
"would",
"wouldn't",
"you",
"you'd",
"you'll",
"you're",
"you've",
"your",
"yours",
"yourself",
"yourselves"
]

keyword_res = [
re.compile(r"^deathtouch$", re.IGNORECASE),
re.compile(r"^defender$", re.IGNORECASE),
re.compile(r"^double strike$", re.IGNORECASE),
re.compile(r"^enchant(?:\s\w+)*$", re.IGNORECASE),
re.compile(r"^equip (?:{.})+$", re.IGNORECASE),
re.compile(r"^first strike$", re.IGNORECASE),
re.compile(r"^flash$", re.IGNORECASE),
re.compile(r"^flying$", re.IGNORECASE),
re.compile(r"^haste$", re.IGNORECASE),
re.compile(r"^hexproof$", re.IGNORECASE),
re.compile(r"^hexproof from(?:\s\w+)*$", re.IGNORECASE),
re.compile(r"^indestructible$", re.IGNORECASE),
re.compile(r"^lifelink$", re.IGNORECASE),
re.compile(r"^menace$", re.IGNORECASE),
re.compile(r"^prowess$", re.IGNORECASE),
re.compile(r"^reach$", re.IGNORECASE),
re.compile(r"^scry \d+$", re.IGNORECASE),
re.compile(r"^trample$", re.IGNORECASE),
re.compile(r"^vigilance$", re.IGNORECASE),
re.compile(r"^attach \d+$", re.IGNORECASE),
re.compile(r"^absorb \d+$", re.IGNORECASE),
re.compile(r"^affinity for(?:\s\w+)*$", re.IGNORECASE),
re.compile(r"^aftermath$", re.IGNORECASE),
re.compile(r"^amplify \d+$", re.IGNORECASE),
re.compile(r"^annihilator \d+$", re.IGNORECASE),
re.compile(r"^aura swap  (?:{.})+$", re.IGNORECASE),
re.compile(r"^bands with other(?:\s\w+)*$", re.IGNORECASE),
re.compile(r"^battle cry$", re.IGNORECASE),
re.compile(r"^bestow (?:{.})+$", re.IGNORECASE),
re.compile(r"^bolster \d+$", re.IGNORECASE),
re.compile(r"^bloodthirst \d+$", re.IGNORECASE),
re.compile(r"^bushido \d+$", re.IGNORECASE),
re.compile(r"^buyback (?:{.})+$", re.IGNORECASE),
re.compile(r"^cascade$", re.IGNORECASE),
re.compile(r"^champion a(?:\s\w+)*$", re.IGNORECASE),
re.compile(r"^changeling$", re.IGNORECASE),
re.compile(r"^cipher$", re.IGNORECASE),
re.compile(r"^clash$", re.IGNORECASE),
re.compile(r"^conspire$", re.IGNORECASE),
re.compile(r"^convoke$", re.IGNORECASE),
re.compile(r"^crew \d+$", re.IGNORECASE),
re.compile(r"^cumulative upkeep (?:{.})+$", re.IGNORECASE),
re.compile(r"^cycling (?:{.})+$", re.IGNORECASE),
re.compile(r"^dash (?:{.})+$", re.IGNORECASE),
re.compile(r"^delve$", re.IGNORECASE),
re.compile(r"^devour \d+$", re.IGNORECASE),
re.compile(r"^dredge \d+$", re.IGNORECASE),
re.compile(r"^echo$", re.IGNORECASE),
re.compile(r"^echo (?:{.})+$", re.IGNORECASE),
re.compile(r"^embalm (?:{.})+$", re.IGNORECASE),
re.compile(r"^entwine (?:{.})+$", re.IGNORECASE),
re.compile(r"^epic$", re.IGNORECASE),
re.compile(r"^evolve$", re.IGNORECASE),
re.compile(r"^evoke (?:{.})+$", re.IGNORECASE),
re.compile(r"^exalted$", re.IGNORECASE),
re.compile(r"^exert$", re.IGNORECASE),
re.compile(r"^exploit$", re.IGNORECASE),
re.compile(r"^extort$", re.IGNORECASE),
re.compile(r"^fabricate \d+$", re.IGNORECASE),
re.compile(r"^fading \d+$", re.IGNORECASE),
re.compile(r"^fateseal \d+$", re.IGNORECASE),
re.compile(r"^flanking$", re.IGNORECASE),
re.compile(r"^flashback (?:{.})+$", re.IGNORECASE),
re.compile(r"^fortify (?:{.})+$", re.IGNORECASE),
re.compile(r"^frenzy \d+$", re.IGNORECASE),
re.compile(r"^graft \d+$", re.IGNORECASE),
re.compile(r"^gravestorm$", re.IGNORECASE),
re.compile(r"^haunt$", re.IGNORECASE),
re.compile(r"^hideaway$", re.IGNORECASE),
re.compile(r"^horsemanship$", re.IGNORECASE),
re.compile(r"^infect$", re.IGNORECASE),
re.compile(r"^kicker (?:{.})+$", re.IGNORECASE),
re.compile(r"^level up$", re.IGNORECASE),
re.compile(r"^living weapon$", re.IGNORECASE),
re.compile(r"^madness (?:{.})+$", re.IGNORECASE),
re.compile(r"^meld$", re.IGNORECASE),
re.compile(r"^miracle (?:{.})+$", re.IGNORECASE),
re.compile(r"^modular \d+$", re.IGNORECASE),
re.compile(r"^morph (?:{.})+$", re.IGNORECASE),
re.compile(r"^multikicker (?:{.})+$", re.IGNORECASE),
re.compile(r"^ninjutsu (?:{.})+$", re.IGNORECASE),
re.compile(r"^(?:\w+\s)*offering$", re.IGNORECASE),
re.compile(r"^overload$", re.IGNORECASE),
re.compile(r"^persist$", re.IGNORECASE),
re.compile(r"^phasing$", re.IGNORECASE),
re.compile(r"^poisonous \d+$", re.IGNORECASE),
re.compile(r"^populate$", re.IGNORECASE),
re.compile(r"^proliferate$", re.IGNORECASE),
re.compile(r"^provoke$", re.IGNORECASE),
re.compile(r"^prowl (?:{.})+$", re.IGNORECASE),
re.compile(r"^rampage \d+$", re.IGNORECASE),
re.compile(r"^rebound$", re.IGNORECASE),
re.compile(r"^recover (?:{.})+$", re.IGNORECASE),
re.compile(r"^reinforce \d+—(?:{.})+$", re.IGNORECASE),
re.compile(r"^renown \d+$", re.IGNORECASE),
re.compile(r"^replicate (?:{.})+$", re.IGNORECASE),
re.compile(r"^retrace \d+$", re.IGNORECASE),
re.compile(r"^ripple$", re.IGNORECASE),
re.compile(r"^scavenge (?:{.})+$", re.IGNORECASE),
re.compile(r"^shadow$", re.IGNORECASE),
re.compile(r"^soulbond$", re.IGNORECASE),
re.compile(r"^soulshift \d+$", re.IGNORECASE),
re.compile(r"^splice onto(?:\s\w+)* (?:{.})+$", re.IGNORECASE),
re.compile(r"^split second$", re.IGNORECASE),
re.compile(r"^storm$", re.IGNORECASE),
re.compile(r"^sunburst$", re.IGNORECASE),
re.compile(r"^suspend \d+—(?:{.})+$", re.IGNORECASE),
re.compile(r"^totem armor$", re.IGNORECASE),
re.compile(r"^transfigure (?:{.})+$", re.IGNORECASE),
re.compile(r"^transmute (?:{.})+$", re.IGNORECASE),
re.compile(r"^(?:\w+)cycling (?:{.})+$", re.IGNORECASE),
re.compile(r"^undying$", re.IGNORECASE),
re.compile(r"^unearth (?:{.})+$", re.IGNORECASE),
re.compile(r"^unleash$", re.IGNORECASE),
re.compile(r"^vanishing \d+$", re.IGNORECASE),
re.compile(r"^wither", re.IGNORECASE)
]

p_stemmer = PorterStemmer()

def massage(text, name):
	card_sentences = []
	name = name.lower()
	text = text.lower()


	# TODO we want to parse out sentences that are just lists of keyword abilities, e.g.
	# "Flying, first strike"
	# As that is really two or more clauses that should be handled as seperate statements

	# Exhance references to the card's name with "self"
	text = text.replace(name, "self")


	#remove parenthetical asides
	text = paren_regex.sub(' ', text)


	#expand contractions
	text = text.replace("won't", "will not")
	text = neg_regex.sub(' not', text)


	#extract keyword sentences like "flying, hexproof, protection from black"
	## This is really hard.
	## Assumptions:
	## 1) Only occurs in the first line.
	##  take from https://en.wikipedia.org/wiki/List_of_Magic:_The_Gathering_keywords
	maybe_keywords_line = text.split("\n")[0]
	all_keywords = False
	keywords = []
	if(',' in maybe_keywords_line):
		all_keywords = True
		maybe_keywords = maybe_keywords_line.split(',')
		#TODO make sure ALL the maybe keywords match!
		for maybe_keyword in maybe_keywords:
			maybe_keyword = maybe_keyword.strip()
			is_keyword = False
			for keyword_re in keyword_res:
				if keyword_re.match(maybe_keyword):
					keywords.append(maybe_keyword)
					is_keyword = True
					break
			if(not is_keyword):
				#if we end up here, we do not have a list of keyword abilities after all
				all_keywords = False
				break

	if(all_keywords):
		text = "\n".join(text.split("\n")[1:]) # remove the first line
		text = "\n".join(keywords) + "\n" + text # re add the abilities one per line


	#remove punctiation except { and } because those form tokens
	text=punc_regex.sub(' ', text)


	# TODO I'd rather split into actual sentences, rather than by lines, using something like
	# https://github.com/RaRe-Technologies/gensim/blob/develop/gensim/summarization/textcleaner.py
	sentences = text.split("\n")

	for sentence in sentences:
		sentence = stop_regex.sub('', sentence)

		# A number of transofmrations. First, remove any text in parenthesis.
		# Second, remove all punctiation

		words = sentence.split() #word_tokenize(sentence)
		# TODO the word_tokenizer take {x} and return '{', 'x', '}'. Undo that.

		# TODO figure out an appropriate stopword set
		stopped_words = [i for i in words if not i in en_stop]
		stemmed_words = [p_stemmer.stem(i) for i in stopped_words]
		card_sentences.append(stemmed_words)

	return card_sentences