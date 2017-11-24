cmake_minimum_required(VERSION 3.8)
set(PROJECT_NAME magique)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

include(conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

set(SOURCE_FILES main.cpp logger.cpp logger.h catalog.cpp catalog.h card.cpp card.h collection.cpp collection.h deck.cpp deck.h)

set(GA2 ga2/ga2.h ga2/ga2Chromosome.h ga2/ga2Chromosome.cpp ga2/ga2Gene.h ga2/ga2Gene.h ga2/ga2Population.h ga2/ga2Population.cpp deck_evaluator.cpp deck_evaluator.h)

add_executable(${PROJECT_NAME} ${GA2} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

# Tests
# add_subdirectory(tests)

