/*
   IGraph library.
   Copyright (C) 2005-2012  The igraph development team <igraph@igraph.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <igraph.h>


void print_graph(igraph_t *pg, const char *fname);

int main(void) {

    igraph_t g;
    FILE *input;
	FILE *fp_dot1, *fp_dot2;

	fp_dot1 = fopen("lgl-1.dot", "w");
	fp_dot2 = fopen("lgl-2.dot", "w");

    /* Turn on attribute handling. */
    igraph_set_attribute_table(&igraph_cattribute_table);

    /* Without names and weights */
	/*
	  # foo
	  bar
	  foobar 5
	  # foobar
	  bat
	  tab
	  # tab
	*/
    input = fopen("igraph_read_graph_lgl-1.lgl", "r");
    if (!input) {
        return 1;
    }
    igraph_read_graph_lgl(&g, input, 0, IGRAPH_ADD_WEIGHTS_NO, 1);
    fclose(input);
    if (!igraph_is_directed(&g)) {
        return 2;
    }
	printf("------- igraph_read_graph_lgl-1.lgl (Without names and weights) --------\n");
	printf("### edge list\n");
    igraph_write_graph_edgelist(&g, stdout);
	igraph_write_graph_dot(&g, fp_dot1);
	/*
	 * print (igraph_t *g, char *name): print name.dot and name.lgl.
	 */
	print_graph(&g, "lgl-1");
    igraph_destroy(&g);

    /* With names and weights */
	/*
	  # foo
	  bar 1
	  foobar 2
	  # foobar
	  bat 10
	  tab
	  # tab
	*/
    input = fopen("igraph_read_graph_lgl-2.lgl", "r");
    if (!input) {
        return 3;
    }
    igraph_read_graph_lgl(&g, input, 0, IGRAPH_ADD_WEIGHTS_YES, 1);
    fclose(input);
    if (!igraph_is_directed(&g)) {
        return 4;
    }
	printf("------- igraph_read_graph_lgl-2.lgl (With names and weights) --------\n");
	printf("### edge list\n");
    igraph_write_graph_ncol(&g, stdout, 0, 0);
	igraph_write_graph_dot(&g, fp_dot2);
	print_graph(&g, "lgl-2-directed-w");
    igraph_destroy(&g);

    /* Same graph, but forcing undirected mode */
    input = fopen("igraph_read_graph_lgl-2.lgl", "r");
    igraph_read_graph_lgl(&g, input, 0, IGRAPH_ADD_WEIGHTS_YES, 0);
    fclose(input);
    if (igraph_is_directed(&g)) {
        return 5;
    }
	printf("------- igraph_read_graph_lgl-2.lgl (undirected and with names and weights) --------\n");
	printf("### edge list\n");
    igraph_write_graph_ncol(&g, stdout, 0, 0);
	print_graph(&g, "lgl-2-undirected-w");
    igraph_destroy(&g);

    /* Erroneous LGL file (empty vertex name) */
    input = fopen("igraph_read_graph_lgl-3.lgl", "r");
    if (!input) {
        return 6;
    }
    igraph_set_error_handler(igraph_error_handler_ignore);
    if (igraph_read_graph_lgl(&g, input, 0, IGRAPH_ADD_WEIGHTS_NO, 1) != IGRAPH_PARSEERROR) {
        return 7;
    }

    return 0;
}
