/*
Graph Printing Routines
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <igraph.h>

void 
print_graph(igraph_t *pg, const char *fname) {
  char *fname_with_ext;
  int fname_len;
  FILE *fp_dot, *fp_lgl;

  fname_len = strlen(fname) + 6;
  fname_with_ext = (char *) malloc(fname_len);
  if (fname_with_ext == NULL) {
	fprintf(stderr, "memory allocation error in print_graph(in graphPrinter.c\n"); 
	exit(10);
  }

  /* LGL file */
  sprintf(fname_with_ext, "%s.lgl", fname);
  if ((fp_lgl = fopen(fname_with_ext, "w")) == NULL) {
	fprintf(stderr, "LGL file open error in print_graph(in grapPrinter.c\n");
	exit(11);
  }
  igraph_write_graph_lgl(pg, fp_lgl, /*names*/ NULL, /*weights*/ NULL, /*isolates*/ 1);


  /* DOT file */
  sprintf(fname_with_ext, "%s.dot", fname);
  if ((fp_dot = fopen(fname_with_ext, "w")) == NULL) {
	fprintf(stderr, "DOT file open error in print_graph(in grapPrinter.c\n");
	exit(12);
  }
  igraph_write_graph_dot(pg, fp_dot);

  fclose(fp_lgl);
  fclose(fp_dot);
}
