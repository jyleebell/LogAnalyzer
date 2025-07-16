cmake_minimum_required(VERSION 3.18)
project(igraph_test)

set(CMAKE_PREFIX_PATH /DATADRIVE1/Projects/BatteryGauge/igraph)
include_directories("/DATADRIVE1/Projects/BatteryGauge/igraph/include"
 "/DATADRIVE1/Projects/BatteryGauge/igraph/include/igraph")

find_package(igraph REQUIRED)

add_compile_options(-g -gdwarf-5)

#add_executable(igraph_tutorial1 tutorial1.c destroy.c)
#add_executable(igraph_tutorial1 tutorial1.c)
#add_executable(igraph_tutorial2 tutorial2.c)
add_executable(iso igraph_isomorphic_vf2.c)
add_executable(iso_lad igraph_subisomorphic_lad.c /DATADRIVE1/Projects/BatteryGauge/LogAnayzer/GraphPrinter/graphPrinter.c)

#target_link_libraries(igraph_tutorial1 PUBLIC igraph::igraph)
#target_link_libraries(igraph_tutorial2 PUBLIC igraph::igraph)
#target_link_libraries(igraph_tutorial3 PUBLIC igraph::igraph)
target_link_libraries(iso PUBLIC igraph::igraph)
target_link_libraries(iso_lad PUBLIC igraph::igraph)


