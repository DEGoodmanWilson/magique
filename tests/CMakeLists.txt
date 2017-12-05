
set(MAGIQUE_TESTS main.cpp
        card.cpp)

add_executable(magique_tests ${MAGIQUE_SOURCE_FILES} ${MAGIQUE_TESTS})

target_link_libraries(magique_tests ${CONAN_LIBS})
