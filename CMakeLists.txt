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

set(GA2_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/ga2/ga2.h
        ${CMAKE_SOURCE_DIR}/ga2/ga2Chromosome.h
        ${CMAKE_SOURCE_DIR}/ga2/ga2Chromosome.cpp
        ${CMAKE_SOURCE_DIR}/ga2/ga2Gene.h
        ${CMAKE_SOURCE_DIR}/ga2/ga2Population.h
        ${CMAKE_SOURCE_DIR}/ga2/ga2Population.cpp
        )

set(DOCOPT_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/docopt/docopt.cpp
        ${CMAKE_SOURCE_DIR}/docopt/docopt.h
        ${CMAKE_SOURCE_DIR}/docopt/docopt_private.h
        ${CMAKE_SOURCE_DIR}/docopt/docopt_util.h
        ${CMAKE_SOURCE_DIR}/docopt/docopt_value.h
        )

set(THREADPOOL_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/ThreadPool/ThreadSafeQueue.h
        ${CMAKE_SOURCE_DIR}/ThreadPool/ThreadPool.h
        )

set(PROGRESSBAR_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/ezETAProgressBar/ezETAProgressBar.hpp
        )

add_executable(${PROJECT_NAME} main.cpp ${GA2_SOURCE_FILES} ${DOCOPT_SOURCE_FILES} ${THREADPOOL_SOURCE_FILES} ${PROGRESSBAR_SOURCE_FILES} ${MAGIQUE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

add_subdirectory(preprocessor)

# Tests
add_subdirectory(tests)

