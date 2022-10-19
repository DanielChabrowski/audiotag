cpmaddpackage("gh:doctest/doctest@2.4.9")

add_executable(unit_tests test_file_reader.cpp)
target_link_libraries(unit_tests PRIVATE doctest)

add_test(NAME UT COMMAND unit_tests -ni)
