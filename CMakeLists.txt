cmake_minimum_required(VERSION 3.18)
project(logAnalyzer)

#set(CMAKE_PREFIX_PATH /DATADRIVE1/Projects/BatteryGauge/igraph)
include_directories(
# "/DATADRIVE1/Projects/BatteryGauge/igraph/include"
# "/DATADRIVE1/Projects/BatteryGauge/igraph/include/igraph"
 "/DATADRIVE1/Projects/BatteryGauge/LogAnayzer"
 "/DATADRIVE1/Projects/BatteryGauge/LogAnayzer/ISO"
 "/DATADRIVE1/Projects/BatteryGauge/LogAnayzer/PatternDB"
 "/DATADRIVE1/Projects/BatteryGauge/LogAnayzer/GraphPrinter"
)
#find_package(igraph REQUIRED)

add_compile_options(-g -gdwarf-5)

#add_executable(igraph_tutorial1 tutorial1.c destroy.c)
add_executable(logAnalyzer 
  codeGen.h
  logAnalyzer.c
  opList.c
  opList.h
  print.c
  print.h
  util.c
  util.h
  find_dep.c
  find_dep.h
  PatternDB/pattern.h
  PatternDB/pattern_gen.c
  #ISO/iso_utils.c
  #ISO/iso.h
  #GraphPrinter/graphPrinter.c
  #GraphPrinter/graph_printer.h
 )
#target_link_libraries(logAnalyzer PUBLIC igraph::igraph)

