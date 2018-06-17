# Using this repo

0. Install build tools (if necessary!)

	```bash
	xcode-select --install
	sudo easy_install pip
	brew install conan
	conan remote add vthiery https://api.bintray.com/conan/vthiery/conan-packages
	```

1. Download MtG card data

   ```bash
   curl https://mtgjson.com/json/AllCards-x.json.zip | tar -xf - -C data
   ```

2. Put your collection of cards as a CSV file in `data/collection.csv` with format:

    ```
    Quantity,Name,Code,PurchasePrice,Foil,Condition,Language,PurchaseData
    ```

    (This is the default file output format for MTG Manager on iOS. See `data/collection_dons.csv` for an example.)

    Make sure that this file contains no basic lands.

3. Install python deps

    ```bash
    cd mechanic_models
    pip install -r requirements.txt
    ```

4. Build mechanics model

   ```bash
   ./build_mode.py
   ```

5. Tag cards

   ```bash
   ./tag_cards.py > ../data/card_tags.json
   ```

6. Download decklists

   ```bash
   curl https://mtgdecks.net/decks/csv/Standard > data/Standard.txt
   curl https://mtgdecks.net/decks/csv/Modern > data/Modern.txt
   curl https://mtgdecks.net/decks/csv/Commander > data/Commander.txt
   curl https://mtgdecks.net/decks/csv/Legacy > data/Legacy.txt
   curl https://mtgdecks.net/decks/csv/Pauper > data/Pauper.txt
   curl https://mtgdecks.net/decks/csv/Vintage > data/Vintage.txt
   curl https://mtgdecks.net/decks/csv/Frontier > data/Frontier.txt
   curl https://mtgdecks.net/decks/csv/Peasant > data/Peasant.txt
   curl https://mtgdecks.net/decks/csv/Highlander > data/Highlander.txt
   ```

7. Calculate conditional probabilities of card pairs

   ```bash
   cd ../deck_analyzer
   python ./ analyze.py > ../data/conditional_probabilities.json
   ```

8. Build genetic algorithm

   ```bash
   cd ../deck_builder
   conan install .
   cmake .
   cmake --build .
   ```

9. Run the genetic algorithm

   ```bash
    ./bin/magique ../data collection.csv
    ```

## Thanks

* To [MTG JSON](https://mtgjson.com/) for the card data.
* To [MTG Decks](https://mtgdecks.net/) for the decklists.
