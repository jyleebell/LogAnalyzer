/*
Graph Printing Routines
*/
#include "graph_printer.h"

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

void
print_graph_line(igraph_t *gp, FILE *fp) {
    igraph_vs_t vs_t;
    igraph_vit_t vit_t;
    igraph_vs_all(&vs_t);

    igraph_vit_create(gp, vs_t, &vit_t);

    while (!IGRAPH_VIT_END(vit_t)) {
      int index = IGRAPH_VIT_GET(vit_t);
      unsigned long temp;
      LogLine *log_lineP;
      int current_lineNo;

      temp = (unsigned long) VAN(gp, "Line", index);
      log_lineP = (LogLine *)temp;
      current_lineNo = log_lineP->lineNo;

      if (log_lineP) printMatchLogLine(stdout, log_lineP, 1);
      IGRAPH_VIT_NEXT(vit_t);
    }
}
