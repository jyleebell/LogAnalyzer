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

#include <igraph.h>

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

igraph_bool_t is_equal_node (
  const igraph_t *target,
  const igraph_t *pattern,
  const igraph_integer_t target_node,
  const igraph_integer_t pattern_node,
  void *arg) {

  printf("target node: %ld, pattern_node: %ld is compared.\n", target_node, pattern_node);
  return true;
}


int main(void) {
    igraph_t target, pattern;
    igraph_bool_t iso;
    igraph_vector_int_t map;
    igraph_vector_int_list_t maps, vmaps;
    igraph_vector_int_t vmap12, vmap21;
    igraph_integer_t i;
	igraph_integer_t sub_iso_count = 0;
    int domainsvec[] = { 0, 2, 8, -1,
                         4, 5, 6, 7, -1,
                         1, 3, 5, 6, 7, 8, -1,
                         0, 2, 8, -1,
                         1, 3, 7, 8, -1, -2
                       };
    igraph_vector_int_list_t domains;
    igraph_vector_int_t v;

    igraph_small(&target, 9, IGRAPH_UNDIRECTED,
                 0, 1, 0, 4, 0, 6,
                 1, 4, 1, 2,
                 2, 3,
                 3, 4, 3, 5, 3, 7, 3, 8,
                 4, 5, 4, 6,
                 5, 6, 5, 8,
                 7, 8,
                 -1);

    igraph_small(&pattern, 5, IGRAPH_UNDIRECTED,
                 0, 1, 0, 4,
                 1, 4, 1, 2,
                 2, 3,
                 3, 4,
                 -1);

	print_graph(&target, "target");
	print_graph(&pattern, "pattern");

    igraph_vector_int_init(&map, 0);
    igraph_vector_int_list_init(&maps, 0);

    igraph_vector_int_init(&vmap12, 0);
    igraph_vector_int_init(&vmap21, 0);
    igraph_vector_int_list_init(&vmaps, 0);

	/*
	Check whether pattern is isomorphic to a subgraph os target. 
	The original LAD implementation by Christine Solnon was used as the basis of this code.

	Arguments:
	  - pattern : The smaller graph, it can be directed or undirected.
	  - target : The bigger graph, it can be directed or undirected.
	  - domains : An integer vector list of NULL. 
				  The length of each vector must match the number of vertices in the pattern graph. 
				  For each vertex, the IDs of the compatible vertices in the target graph are listed.
	  - iso : Pointer to a boolean, or a null pointer. 
			  If not a null pointer, then the boolean is set to true if a subgraph isomorphism is found, and to false otherwise.
	  - map : Pointer to a vector or a null pointer. 
			  If not a null pointer and a subgraph isomorphism is found, 
			  the matching vertices from the target graph are listed here, for each vertex (in vertex ID order) from the pattern graph.
	  - maps : Pointer to a list of integer vectors or a null pointer. 
			   If not a null pointer, then all subgraph isomorphisms are stored in the vector list, in igraph_vector_int_t objects.
	  - induced : Boolean, whether to search for induced matching subgraphs.
	  - time_limit : Processor time limit in seconds. Supply zero here for no limit. If the time limit is over, then the function signals an error.
	Returns:
	  - Error code
	*/

    igraph_subisomorphic_lad(&pattern, &target, /*domains=*/ NULL, &iso, &map,
                             &maps, /*induced=*/ false, /*time_limit=*/ 0);
    if (!iso) {
        return 1;
    }
	printf("\n\n----- lad (not induced) : target vertices -----\n");
    print_maps(&map, &maps);

	igraph_subisomorphic_lad(&pattern, &target, /*domains=*/ NULL, &iso, &map,
                             &maps, /*induced=*/ true, /*time_limit=*/ 0);
    if (!iso) {
        return 1;
    }
	printf("\n\n----- lad (induced) : target verices -----\n");
    print_maps(&map, &maps);


	/*
    igraph_subisomorphic_vf2
	Decides whether a subgraph of graph1 is isomorphic to graph2. 
	It uses igraph_get_subisomorphisms_vf2_callback() .
	Arguments:
	  - graph1 : The first input graph, may be directed or undirected. This is supposed to be the larger graph.
	  - graph2 : The second input graph, it must have the same directedness as graph1. 
	             This is supposed to be the smaller graph.
	  - vertex_color1 : An optional color vector for the first graph.  
						If color vectors are given for both graphs, then the subgraph isomorphism is 
						calculated on the colored graphs; i.e. two vertices can match 
						only if their color also matches. Supply a null pointer here if your graphs are not colored.
	  - vertex_color2 : An optional color vector for the second graph. See the previous argument for explanation.
	  - edge_color1 : An optional edge color vector for the first graph. 
					  The matching edges in the two graphs must have matching colors as well.  
					  Supply a null pointer here if your graphs are not edge-colored.
	  - edge_color2 : The edge color vector for the second graph.
	  - iso : Pointer to a boolean. The result of the decision problem is stored here.
	  - map12 : Pointer to a vector or NULL. 
	            If not NULL, then an isomorphic mapping from graph1 to graph2 is stored here.
	  - map21 : Pointer to a vector ot NULL. 
	            If not NULL, then an isomorphic mapping from graph2 to graph1 is stored here.
	  - node_compat_fn : A pointer to a function of type igraph_isocompat_t. 
	                     This function will be called by the algorithm to determine 
						 whether two nodes are compatible.
	  - edge_compat_fn : A pointer to a function of type igraph_isocompat_t. 
	                     This function will be called by the algorithm to determine 
						 whether two edges are compatible.
	  - arg : Extra argument to supply to functions node_compat_fn and edge_compat_fn.
	Returns:
	  - Error code.
	*/

	/*
	igraph_isocompat_t — Callback type, called to check whether two vertices or edges are compatible

	typedef igraph_bool_t igraph_isocompat_t(const igraph_t *graph1,
        const igraph_t *graph2,
        const igraph_integer_t g1_num,
        const igraph_integer_t g2_num,
        void *arg);

	- VF2 (subgraph) isomorphism functions can be restricted by defining relations 
	  on the vertices and/or edges of the graphs, and then checking whether the vertices (edges) match 
	  according to these relations.

	- This feature is implemented by two callbacks, one for vertices, one for edges. 
	  Every time igraph tries to match a vertex (edge) of the first (sub)graph to a vertex of the second graph, 
	  the vertex (edge) compatibility callback is called. 
	  The callback returns a logical value, giving whether the two vertices match.

	- Both callback functions are of type igraph_isocompat_t.

	Arguments:
	- graph1: The first graph.
	- graph2: The second graph.
	- g1_num: The id of a vertex or edge in the first graph.
	- g2_num: The id of a vertex or edge in the second graph.
	- arg: Extra argument to pass to the callback functions.

	Returns:
	- Logical scalar, whether vertex (or edge) g1_num in graph1 is compatible 
	  with vertex (or edge) g2_num in graph2.


	  igraph_isocompat_t *node_compat_fn;
	  igraph_bool_t is_equal_node (
		const igraph_t *target,
		const igraph_t *pattern,
		const igraph_integer_t target_node;
		const igraph_integer_t pattern_node;
		void *arg) {
		...
	  }

	  node_compat_fn = is_equal_node;
	*/


    igraph_subisomorphic_vf2(
				  /* graph1= */ &target, 
				  /* graph2= */ &pattern, 
				  /* vertex_color1= */ NULL, 
				  /* vertex_color1= */ NULL, 
				  /* edge_color1= */ NULL, 
				  /* edge_color1= */ NULL, 
				  /* iso= */ &iso, 
				  /* map12= */ &vmap12, 
				  /* map21= */ &vmap21, 
				  /* node_compat_fn= */ is_equal_node, 
				  /* edge_compat_fn= */ NULL,
				  /* arg= */ 0);

	/*
	igraph_count_subisomorphisms_vf2
	Count the number of isomorphisms between subgraphs of graph1 and graph2. This function uses igraph_get_subisomorphisms_vf2_callback() .
	Arguments:
	  - graph1 : The first input graph, may be directed or undirected. This is supposed to be the larger graph.
	  - graph2 : The second input graph, it must have the same directedness as graph1 . This is supposed to be the smaller graph.
	  - vertex_color1 : An optional color vector for the first graph.  If color vectors are given for both graphs, 
						then the subgraph isomorphism is calculated on the colored graphs; 
						i.e. two vertices can match only if their color also matches. Supply a null pointer here if your graphs are not colored.
	  - vertex_color2 : An optional color vector for the second graph. See the previous argument for explanation.
	  - edge_color1 : An optional edge color vector for the first graph. The matching edges in the two graphs must have matching colors as well.  
					  Supply a null pointer here if your graphs are not edge-colored.
	  - edge_color2 : The edge color vector for the second graph.
	  - count : Pointer to an integer. The number of subgraph isomorphisms is stored here.
	  - node_compat_fn : A pointer to a function of type igraph_isocompat_t . 
						 This function will be called by the algorithm to determine whether two nodes are compatible.
	  - edge_compat_fn : A pointer to a function of type igraph_isocompat_t . 
						 This function will be called by the algorithm to determine whether two edges are compatible.
	  - arg : Extra argument to supply to functions node_compat_fn and edge_compat_fn .
	Returns:
	  - Error code.

	*/
	igraph_count_subisomorphisms_vf2(
				  /* graph1= */ &target, 
				  /* graph2= */ &pattern, 
				  /* vertex_color1= */ NULL, 
				  /* vertex_color1= */ NULL, 
				  /* edge_color1= */ NULL, 
				  /* edge_color1= */ NULL, 
				  /* count= */ &sub_iso_count, 
				  /* node_compat_fn= */ NULL, 
				  /* edge_compat_fn= */ NULL,
				  /* arg= */ 0);


	/*
	igraph_get_subisomorphisms_vf2 collects all isomorphic mappings of graph2 to a subgraph of graph1. 
	It uses the igraph_get_subisomorphisms_vf2_callback() function. The graphs should be simple.
	Arguments:
	  - graph1 : The first input graph, may be directed or undirected. This is supposed to be the larger graph.
	  - graph2 : The second input graph, it must have the same directedness as graph1. 
	             This issupposed to be the smaller graph.
	  - vertex_color1 : An optional color vector for the first graph.  
						If color vectors are given for both graphs, then the subgraph isomorphism 
						is calculated on the colored graphs; i.e. two vertices can match 
						only if their color also matches. Supply a null pointer here if your graphs are not colored.
	  - vertex_color2 : An optional color vector for the second graph. See the previous argument for explanation.
	  - edge_color1 : An optional edge color vector for the first graph. 
	                  The matching edges in the two graphs must have matching colors as well.  
					  Supply a null pointer here if your graphs are not edge-colored.  
	  - edge_color2 : The edge color vector for the second graph.
	  - maps : Pointer to a list of integer vectors. On return it contains pointers to igraph_vector_int_t objects, 
			   each vector is an isomorphic mapping of graph2 to a subgraph of graph1.
	  - node_compat_fn : A pointer to a function of type igraph_isocompat_t. 
						 This function will be called by the algorithm to determine 
						 whether two nodes are compatible.
	  - edge_compat_fn : A pointer to a function of type igraph_isocompat_t. 
						This function will be called by the algorithm to determine whether two edges are compatible.
	  - arg : Extra argument to supply to functions node_compat_fn and edge_compat_fn.
	Returns:
	  - Error code.
	*/
	igraph_get_subisomorphisms_vf2(
				  /* graph1= */ &target, 
				  /* graph2= */ &pattern, 
				  /* vertex_color1= */ NULL, 
				  /* vertex_color1= */ NULL, 
				  /* edge_color1= */ NULL, 
				  /* edge_color1= */ NULL, 
				  /* maps= */ &vmaps, 
				  /* node_compat_fn= */ is_equal_node, 
				  /* edge_compat_fn= */ NULL,
				  /* arg= */ 0);
	printf("\n\n----- vf2 (No. of subisomorphic graph = %ld)-----\n", sub_iso_count);
	printf("########## map 1 (target, larger graph) ==> 2(pattern, smaller graph)\n");
    print_map(&vmap12);
	printf("########## map 2 (pattern, smaller graph) ==> 1(target, larger graph)\n");
    print_map(&vmap21);
	printf("########## all mappings \n");
    print_maps(&vmap21, &vmaps);


    igraph_vector_int_list_init(&domains, 0);
    i = 0;
    igraph_vector_int_init(&v, 0);
    while (1) {
        if (domainsvec[i] == -2) {
            break;
        } else if (domainsvec[i] == -1) {
            igraph_vector_int_list_push_back_copy(&domains, &v);
            igraph_vector_int_clear(&v);
        } else {
            igraph_vector_int_push_back(&v, domainsvec[i]);
        }
        i++;
    }
    igraph_vector_int_destroy(&v);

    igraph_subisomorphic_lad(&pattern, &target, &domains, &iso, &map, &maps,
                             /*induced=*/ false, /*time_limit=*/ 0);

    if (!iso) {
        return 3;
    }
	printf("\n\n----- lad (domain, not induced) -----\n");
    print_maps(&map, &maps);

    igraph_vector_int_list_destroy(&domains);
    igraph_vector_int_destroy(&map);
    igraph_vector_int_list_destroy(&maps);

    igraph_destroy(&pattern);
    igraph_destroy(&target);


    return 0;
}
