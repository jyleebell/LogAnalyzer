//
// pattern.h
// Created by JBNU 
//

typedef struct pattern_t {
  char fun[20];
  unsigned long start_pc;
  unsigned long end_pc;
  
  unsigned long start_lineno;
  unsigned long end_lineno;
} PATTERN;

typedef struct pattern_node_t {
  PATTERN *pattern_data;
  struct pattern_node_t *link;
} PATTERN_NODE;

