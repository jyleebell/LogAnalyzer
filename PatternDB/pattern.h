//
// pattern.h
// Created by JBNU 
//

#ifndef pattern_h
#define pattern_h

#define MAX_ARGS 4

typedef enum pattern_op_type {
  ADD_2B = 0,
  ADD_4B,	/* 1 */
  SUB_2B,	/* 2 */
  SUB_4B,	/* 3 */
  RSHIFT_2B,	/* 4 */
  RSHIFT_4B,	/* 5 */
  LSHIFT_2B,	/* 6 */
  LSHIFT_4B,	/* 7 */
  MUL_2B,	/* 8 */
  MUL_4B,	/* 9 */
  DIV_2B,	/* 10 */
  DIV_4B,	/* 11 */
  SQRT,	/* 12 */
  EXP	/* 13 */
} PATTERN_OP_TYPE;

typedef struct pattern_t {
  char fun[20];
  PATTERN_OP_TYPE type;
  unsigned long start_pc;
  unsigned long end_pc;

  /* Max. number of operands is 4 bytes. */
  /* Line number offsets from the first line of a pattern */
  int op0_offset[MAX_ARGS];
  int op1_offset[MAX_ARGS];

} PATTERN;

typedef struct pattern_node_t {
  PATTERN *pattern_data;
  struct pattern_node_t *link;
} PATTERN_NODE;


#define GET_PATTERN_TYPE(pn) pn->pattern_data->type
#define GET_PATTERN_START_PC(pn) pn->pattern_data->start_pc
#define GET_PATTERN_END_PC(pn) pn->pattern_data->end_pc
#define GET_PATTERN_NAME(pn) pn->pattern_data->fun
#define GET_PATTERN_OFFSET0(pn, index) pn->pattern_data->op0_offset[index]
#define GET_PATTERN_OFFSET1(pn, index) pn->pattern_data->op1_offset[index]


void parse_pattern_file(char *fname);
void print_pattern_list(int debug);
#endif
