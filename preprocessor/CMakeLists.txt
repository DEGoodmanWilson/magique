set(magique_scrape_files scrape.cpp)
add_executable(magique_scraper ${MAGIQUE_SOURCE_FILES} ${magique_scrape_files})
target_link_libraries(magique_scraper ${CONAN_LIBS})


set(magique_preprocess_FILES main.cpp)
add_executable(magique_preprocess ${MAGIQUE_SOURCE_FILES} ${magique_preprocess_FILES})
target_link_libraries(magique_preprocess ${CONAN_LIBS})



set(GENERATE_TRAINING_DATA generate_training_data.cpp)
add_executable(generate_training_data ${MAGIQUE_SOURCE_FILES} ${GENERATE_TRAINING_DATA})
target_link_libraries(generate_training_data ${CONAN_LIBS})