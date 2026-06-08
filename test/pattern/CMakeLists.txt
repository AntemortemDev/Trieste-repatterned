add_executable(pattern_test
    pattern_test.cc
)
enable_warnings(pattern_test)
target_link_libraries(pattern_test trieste::trieste)

add_executable(pattern_gen
    pattern_gen.cc
)
enable_warnings(pattern_gen)
target_link_libraries(pattern_gen trieste::trieste)