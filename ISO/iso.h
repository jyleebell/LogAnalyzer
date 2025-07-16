#ifndef iso_h
#define iso_h
#include <igraph.h>

#ifndef pattern_h
#define pattern_h
#include "pattern.h"
#endif

typedef struct isoResult {
  int log_no;
  int pattern_no;
  PATTERN_NODE *pn;
  int start;
  int end;
} isoResult;

typedef struct isoReNode {
  isoResult result;
  struct isoReNode *link;
} isoReNode;

void print_graph(igraph_t *pg, const char *fname);
void print_maps(igraph_vector_int_t *map, igraph_vector_int_list_t *maps);
void print_map(igraph_vector_int_t *map);
igraph_bool_t is_equal_node (
  const igraph_t *target,
  const igraph_t *pattern,
  const igraph_integer_t target_node,
  const igraph_integer_t pattern_node,
  void *arg);

void print_iso(igraph_t *log_g, igraph_vector_int_t map, char *fun, int log_no, int pattern_no);

void setISOResultNode(igraph_t *log_g, isoReNode *n, igraph_vector_int_t map, int log_no, int pattern_no, PATTERN_NODE *pn);
#endif


