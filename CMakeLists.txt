cmake_minimum_required(VERSION 3.8)
set(PROJECT_NAME magique)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

include(conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

set(MAGIQUE_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/magique/catalog.cpp
        ${CMAKE_SOURCE_DIR}/magique/catalog.h
        ${CMAKE_SOURCE_DIR}/magique/card.cpp
        ${CMAKE_SOURCE_DIR}/magique/card.h
        ${CMAKE_SOURCE_DIR}/magique/collection.cpp
        ${CMAKE_SOURCE_DIR}/magique/collection.h
        ${CMAKE_SOURCE_DIR}/magique/deck.cpp
        ${CMAKE_SOURCE_DIR}/magique/deck.h
        ${CMAKE_SOURCE_DIR}/magique/deck_evaluator.cpp
        ${CMAKE_SOURCE_DIR}/magique/deck_evaluator.h
        ${CMAKE_SOURCE_DIR}/magique/interactions.cpp
        ${CMAKE_SOURCE_DIR}/magique/interactions.h
        )

set(GA2 ga2/ga2.h
        ga2/ga2Chromosome.h
        ga2/ga2Chromosome.cpp
        ga2/ga2Gene.h
        ga2/ga2Gene.h
        ga2/ga2Population.h
        )

add_executable(${PROJECT_NAME} main.cpp ${GA2} ${MAGIQUE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

add_subdirectory(preprocessor)

# Tests
add_subdirectory(tests)

