cmake_minimum_required(VERSION 3.15)
project(top-trees)

set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(CMAKE_CXX_FLAGS "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O4 -DNDEBUG")

include_directories(include)
include_directories(include/top_tree)

set(IMPL_FILES ${IMPL_FILES}
src/cover_level.cpp
src/two_edge_connected.cpp
src/two_edge_cluster.cpp
src/find_size.cpp
src/find_first_label.cpp
)

set(TEST_FILES ${TEST_FILES}
test/tree_tests/tree_test.cpp
test/toptree_tests/max_weight_in_component_test.cpp
test/toptree_tests/max_weight_on_path_test.cpp
test/toptree_tests/add_common_weight_test.cpp
test/toptree_tests/orientation_invariant_test.cpp
test/toptree_tests/diameter_test.cpp
test/2_edge_tests/find_size_test.cpp
test/2_edge_tests/find_first_label_test.cpp
test/2_edge_tests/two_edge_connected_test.cpp
test/min_span_forest_tests/min_span_forest_test.cpp
)

#Main target
add_executable(benchmark_mst src/benchmark/benchmark_mst.cpp)
add_executable(benchmark_connectivity src/benchmark/benchmark_connectivity.cpp)
add_executable(benchmark_2_edge src/benchmark/benchmark_2_edge.cpp)
add_executable(benchmark_connectivity_semi_splay src/benchmark/benchmark_connectivity.cpp)
add_executable(benchmark_2_edge_semi_splay src/benchmark/benchmark_2_edge.cpp)

target_sources(benchmark_2_edge PRIVATE ${IMPL_FILES})
target_sources(benchmark_2_edge_semi_splay PRIVATE ${IMPL_FILES})

target_compile_definitions(benchmark_connectivity_semi_splay PUBLIC SEMI_SPLAY=1)
target_compile_definitions(benchmark_2_edge_semi_splay PUBLIC SEMI_SPLAY=1)

add_subdirectory(src/lib/Catch2)
#Removes extra CTest targets
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
#Add Ctest and Catch to CMake
include(CTest)
include(Catch)
enable_testing()

#Create tests target and add it to test list
add_executable(tests ${TEST_FILES} ${IMPL_FILES})
target_link_libraries(tests PRIVATE Catch2 Catch2WithMain)
catch_discover_tests(tests)
