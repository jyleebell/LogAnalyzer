//
// pattern.h
// Created by JBNU 
//

#ifndef pattern_h
#define pattern_h

#define MAX_ARGS 4

typedef struct pattern_t {
  char fun[20];
  unsigned long start_pc;
  unsigned long end_pc;

  int op0_offset[MAX_ARGS];
  int op1_offset[MAX_ARGS];

} PATTERN;

typedef struct pattern_node_t {
  PATTERN *pattern_data;
  struct pattern_node_t *link;
} PATTERN_NODE;


#define GET_PATTERN_START_PC(pn) pn->pattern_data->start_pc
#define GET_PATTERN_END_PC(pn) pn->pattern_data->end_pc
#define GET_PATTERN_NAME(pn) pn->pattern_data->fun
#define GET_PATTERN_OFFSET0(pn, index) pn->pattern_data->op0_offset[index]
#define GET_PATTERN_OFFSET1(pn, index) pn->pattern_data->op1_offset[index]


void parse_pattern_file(char *fname);
void print_pattern_list(int debug);
#endif
