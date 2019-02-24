# Using this repo

0. Install build tools (if necessary!)

	```bash
	xcode-select --install
	sudo easy_install pip
	brew install conan
	conan remote add vthiery https://api.bintray.com/conan/vthiery/conan-packages
  pip install virtualenv
	virtualenv --python=python3 venv
  source venv/bin/activate
	```

1. Download MtG card data

   ```bash
   ./run.sh
   ```

2. Put your collection of cards as a JSON file in `data/collection.json` as an object with card names as keys, and the quantity you own as the values. For example, if you own four `Walking Balista`s:

    ```
    {
      "Walking Ballista": 4
    }
    ```

    Be sure to use canonical card names, as reported on Gatherer or Scryfall. Split cards and two-sided cards will be formatted like "Driven // Despair" (Notice the spaces around the double-slashes).

    If you use MTG Manager on iOS, there is a script in `data_builders/collection/mtg_manager_csv_to_json.py` that will convert collections exported from MTG Manager to the required format

8. Build genetic algorithm

   ```bash
   cd ../deck_builder
   conan install .
   cmake .
   cmake --build .
   ```

9. Run the genetic algorithm

   ```bash
    ./bin/magique ../data collection.json
    ```

10. Deactivate virtual environment

    ```bash
    deactivate
    ```

## Thanks

* To [MTG JSON](https://mtgjson.com/) for the card data.
* To [MTG Decks](https://mtgdecks.net/) for the decklists.
* To [EDHREC](https://edhrec.com/) for the awesome Commander data.
