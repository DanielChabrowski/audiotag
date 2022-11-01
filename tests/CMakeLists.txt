cpmaddpackage("gh:doctest/doctest@2.4.9")

add_executable(unit_tests test_mpeg_reader.cpp)
target_link_libraries(unit_tests PRIVATE doctest audiotag)

if(BUILD_STATIC AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_target_properties(unit_tests PROPERTIES LINK_SEARCH_START_STATIC ON)
    set_target_properties(unit_tests PROPERTIES LINK_SEARCH_END_STATIC ON)

    target_link_options(
        unit_tests
        PRIVATE -static-libgcc -static-libstdc++ -static
    )
endif()

target_compile_definitions(
    unit_tests
    PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_LIST_DIR}/test_data"
)

add_custom_target(
    run_tests
    COMMAND unit_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS unit_tests
)

add_custom_target(
    run_mips_tests
    COMMAND scripts/mips-test.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
