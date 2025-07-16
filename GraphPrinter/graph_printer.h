#ifndef graph_printer_h
#define graph_print_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <igraph.h>

#include "print.h"
#include "opList.h"


void print_graph(igraph_t *pg, const char *fname);
void print_graph_line(igraph_t *gp, FILE *fp);

#endif
