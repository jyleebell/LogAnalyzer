/* -*- mode: C -*-  */
/*
   IGraph library.
   Copyright (C) 2012  Gabor Csardi <csardi.gabor@gmail.com>
   334 Harvard street, Cambridge, MA 02139 USA

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA

*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "codeGen.h"
#include "util.h"
#include "print.h"
#include "opList.h"
#include "pattern.h"
#include "iso.h"


void print_graph(igraph_t *pg, const char *fname);

/*
  igraph_vector_int_t: array
  igraph_vector_int_list_t: array of int vectors
*/
void print_maps(igraph_vector_int_t *map, igraph_vector_int_list_t *maps) {
    igraph_integer_t n, i;
	printf("\t**** igraph_vector_int_print\n");
    igraph_vector_int_print(map);
    n = igraph_vector_int_list_size(maps);
	printf("\t\t**** igraph_vector_int_list_print\n");
    for (i = 0; i < n; i++) {
        igraph_vector_int_t *v = igraph_vector_int_list_get_ptr(maps, i);
        igraph_vector_int_print(v);
    }
    igraph_vector_int_list_clear(maps);
}

void print_map(igraph_vector_int_t *map) {
    igraph_integer_t n, i;
	printf("\t**** igraph_vector_int_print\n");
    igraph_vector_int_print(map);
}

#define GET_OPERATOR(lineP) lineP->opP->op

igraph_bool_t is_equal_node (
  const igraph_t *target,
  const igraph_t *pattern,
  const igraph_integer_t target_node,
  const igraph_integer_t pattern_node,
  void *arg) {
  unsigned long temp;
  LogLine *log_lineP, *pattern_lineP;
  OPERATOR log_op, pattern_op;


  temp = (unsigned long) VAN(target, "Line", target_node);
  log_lineP = (LogLine *) temp;
  // printf(">>> target node: %ld, pattern_node: %ld is compared.\n", target_node, pattern_node);
  // printLogLine(stdout, log_lineP, 1);

  temp = (unsigned long) VAN(pattern, "Line", pattern_node);
  pattern_lineP = (LogLine *) temp;
  // printLogLine(stdout, pattern_lineP, 1);

  log_op = GET_OPERATOR(log_lineP);
  pattern_op = GET_OPERATOR(pattern_lineP);

  if (log_op == pattern_op) {
	if (
		log_op == ADD ||
		log_op == SUB ||
		log_op == MULT ||
		log_op == DIV ||
		log_op == RSHIFT ||
		log_op == LSHIFT ||
		log_op == OR ||
		log_op == AND ||
		log_op == XOR ||
		log_op == NOT 
	  ) {
		return true;
	} else if (log_op == MOVE || log_op == ALUMOVE) {
	  return true;
	}
  } else {
	return false;
  }

  return false;
}


void
print_iso(igraph_t *log_g, igraph_vector_int_t map, char *fun, int log_no, int pattern_no) {
  igraph_integer_t size, i, vid;
  unsigned long temp;
  LogLine *log_lineP;
  OPERATOR log_op, pattern_op;

  size = igraph_vector_int_size(&map);

  printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
  printf("In log %d, %s_%d matches to : \n", log_no, fun, pattern_no);
  for (i = 0; i < size; i++) {
	vid = VECTOR(map)[i];
	temp = (unsigned long) VAN(log_g, "Line", vid);
	log_lineP = (LogLine *) temp;
	if (log_lineP) printMatchLogLine(stdout, log_lineP, 1);
  }
  printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
}

void
setISOResultNode(igraph_t *log_g, isoReNode *n, igraph_vector_int_t map, int log_no, int pattern_no, PATTERN_NODE *pn) {
  igraph_integer_t size, i, vid;
  igraph_integer_t  max, min;
  unsigned long temp;
  LogLine *log_lineP;

  (n->result).log_no = log_no;
  (n->result).pattern_no = pattern_no;
  (n->result).pn = pn;
  
  size = igraph_vector_int_size(&map);
  max = -1;
  min = 65536;
  for (i = 0; i < size; i++) {
	vid = VECTOR(map)[i];
	if (vid == -1) continue;
	temp = (unsigned long) VAN(log_g, "Line", vid);
	log_lineP = (LogLine *) temp;
	if (min > log_lineP->lineNo) 
	  min = log_lineP->lineNo;
	if ((int) max < (int) log_lineP->lineNo)
	  max = log_lineP->lineNo;
  }
  (n->result).start = min;
  (n->result).end = max;
}
