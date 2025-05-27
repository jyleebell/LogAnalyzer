//
// pattern.h
// Created by JBNU 
//

#include <igraph.h>

typedef struct pattern_t {
  char fun[20];
  unsigned long start_pc;
  unsigned long end_pc;
  
  unsigned long start_lineno;
  unsigned long end_lineno;

  igraph_t pg;
  igraph_graph_list_t complist;
} PATTERN;

typedef struct pattern_node_t {
  PATTERN *pattern_data;
  struct pattern_node_t *link;
} PATTERN_NODE;


void parse_pattern_file(char *fname);
void print_pattern_list();
