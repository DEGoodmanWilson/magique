set(magique_scrape_files scrape.cpp)
add_executable(magique_scraper ${magique_scrape_files})
target_link_libraries(magique_scraper ${CONAN_LIBS})


set(magique_preprocess_FILES main.cpp)
add_executable(magique_preprocess ${magique_preprocess_FILES})
target_link_libraries(magique_preprocess ${CONAN_LIBS})
