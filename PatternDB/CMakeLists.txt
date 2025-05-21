cmake_minimum_required(VERSION 3.18)
project(test_pattern_gen)

set(CMAKE_PREFIX_PATH /DATADRIVE1/Projects/BatteryGauge/igraph)
include_directories(
 "/DATADRIVE1/Projects/BatteryGauge/igraph/include"
 "/DATADRIVE1/Projects/BatteryGauge/igraph/include/igraph"
 "/DATADRIVE1/Projects/BatteryGauge/LogAnayzer"
)
find_package(igraph REQUIRED)

add_compile_options(-g -gdwarf-5)

#add_executable(igraph_tutorial1 tutorial1.c destroy.c)
add_executable(test_pattern_gen 
  pattern.h
  pattern_gen.c
  ../util.c
)

target_compile_definitions(test_pattern_gen PRIVATE UNIT_TEST_PATTERN_DB)

target_link_libraries(test_pattern_gen PUBLIC igraph::igraph)

