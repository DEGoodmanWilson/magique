cmake_minimum_required(VERSION 3.8)
set(PROJECT_NAME magique)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

include(conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

set(SOURCE_FILES main.cpp)

set(MODELS logger.cpp logger.h catalog.cpp catalog.h card.cpp card.h collection.cpp collection.h)

set(CONTROLLERS )

add_executable(${PROJECT_NAME} ${CONTROLLERS} ${MODELS} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

# Tests
# add_subdirectory(tests)

