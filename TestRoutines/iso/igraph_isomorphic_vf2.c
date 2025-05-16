/* -*- mode: C -*-  */
/*
   IGraph library.
   Copyright (C) 2009-2012  Gabor Csardi <csardi.gabor@gmail.com>
   334 Harvard st, Cambridge MA, 02139 USA

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
#include <stdio.h>
#include <stdlib.h>

int main(void) {

	FILE *fp1, *fp2;

    igraph_t ring1, ring2;
    igraph_vector_int_t color1, color2;
    igraph_vector_int_t perm;
    igraph_bool_t iso;
    igraph_integer_t count;
    igraph_integer_t i;

	fp1 = fopen("ring1.dot", "w");
	fp2 = fopen("ring2.dot", "w");

    igraph_rng_seed(igraph_rng_default(), 12345);

    igraph_ring(&ring1, 100, /*directed=*/ 0, /*mutual=*/ 0, /*circular=*/1);
    igraph_vector_int_init_range(&perm, 0, igraph_vcount(&ring1));

	/*
	Shuffles a vector in-place
	*/
    igraph_vector_int_shuffle(&perm);

	/*
	igraph_permute_vertices creates a new graph from the input graph by permuting its
	vertices according to the specified mapping. Call this function with the
	output of igraph_canonical_permutation() to create the canonical form of a graph.
	Arguments:
	  - graph : The input graph.
	  - res : Pointer to an uninitialized graph object. The new graph is created here.
	  - permutation : The permutation to apply. Vertex 0 is mapped to the first element of the vector, 
					  vertex 1 to the second, etc.
	*/
    igraph_permute_vertices(&ring1, &ring2, &perm);



    /* Everything has the same colors */
    igraph_vector_int_init(&color1, igraph_vcount(&ring1));
    igraph_vector_int_init(&color2, igraph_vcount(&ring2));

	/*
	This function performs the VF2 algorithm via calling igraph_get_isomorphisms_vf2_callback() .
	Note that this function cannot be used for deciding subgraph isomorphism, use igraph_subisomorphic_vf2() for that.
	Arguments:
	- graph1 : The first graph, may be directed or undirected.
	- graph2 : The second graph. It must have the same directedness as graph1 , otherwise an error is reported.
	- vertex_color1 : An optional color vector for the first graph.  If color vectors are given for both graphs,
					  then the isomorphism is calculated on the colored graphs; i.e. two vertices can match only 
					  if their color also matches. Supply a null pointer here if your graphs are not colored.
	- vertex_color2 : An optional color vector for the second graph. See the previous argument for explanation.
	- edge_color1 : An optional edge color vector for the first graph. The matching edges in the two graphs must 
					have matching colors as well.  Supply a null pointer here if your graphs are not edge-colored.
	- edge_color2 : The edge color vector for the second graph.
	- iso : Pointer to a logical constant, the result of the algorithm will be placed here.
	- map12 : Pointer to an initialized vector or a NULL pointer. If not a NULL pointer then the
			  mapping from graph1 to graph2 is stored here. If the graphs are not isomorphic then 
			  the vector is cleared (i.e. has zero elements).
	- map21 : Pointer to an initialized vector or a NULL pointer. If not a NULL pointer then the mapping from graph2 
			  to graph1 is stored here. If the graphs are not isomorphic then the vector is cleared (i.e. has zero elements).
	- node_compat_fn : A pointer to a function of type igraph_isocompat_t. This function will be called by 
					   the algorithm to determine whether two nodes are compatible.  
    - edge_compat_fn : A pointer to a function of type igraph_isocompat_t. This function will be called by 
					  the algorithm to determine whether two edges are compatible.
	- arg : Extra argument to supply to functions node_compat_fn and edge_compat_fn .
	*/
    // nigraph_isomorphic_vf2(&ring1, &ring2, &color1, &color2, 0, 0, &iso, 0, 0, 0, 0, 0);
    igraph_isomorphic_vf2(&ring1, &ring2, NULL, NULL, 0, 0, &iso, 0, 0, 0, 0, 0);
    if (!iso) {
        fprintf(stderr, "Single color failed.\n");
        return 1;
    }

    /* Two colors, just counting : 100 vertices make isomorphism */
    for (i = 0; i < igraph_vector_int_size(&color1); i += 2) {
        VECTOR(color1)[i] = VECTOR(color2)[VECTOR(perm)[i]] = 1;
    }
    igraph_count_isomorphisms_vf2(&ring1, &ring2, &color1, &color2, 0, 0, &count, 0, 0, 0);
    // igraph_count_isomorphisms_vf2(&ring1, &ring2, NULL, NULL, 0, 0, &count, 0, 0, 0);
    if (count != 100) {
        fprintf(stderr, "Count with two colors failed, expected 100, got %" IGRAPH_PRId ".\n", count);
        return 2;
    }

    igraph_destroy(&ring1);
    igraph_destroy(&ring2);
    igraph_vector_int_destroy(&color2);
    igraph_vector_int_destroy(&perm);


    /* Two colors, count subisomorphisms */
    igraph_ring(&ring1, 100, /*directed=*/ 0, /*mutual=*/ 0, /*circular=*/0);
    igraph_ring(&ring2, 80, /*directed=*/ 0, /*mutual=*/ 0, /*circular=*/0);

	/*
	igraph_write_graph_dot(&ring1, fp1);
	igraph_write_graph_dot(&ring2, fp2);
	*/

	igraph_write_graph_lgl(&ring1, fp1, NULL, NULL, 1);
	igraph_write_graph_lgl(&ring2, fp2, NULL, NULL, 1);

    igraph_vector_int_init(&color2, igraph_vcount(&ring2));
    for (i = 0; i < igraph_vector_int_size(&color1); i += 2) {
        VECTOR(color1)[i]   = 0;
        VECTOR(color1)[i + 1] = 1;
    }
    for (i = 0; i < igraph_vector_int_size(&color2); i += 2) {
        VECTOR(color2)[i]   = 0;
        VECTOR(color2)[i + 1] = 1;
    }


    igraph_count_subisomorphisms_vf2(&ring1, &ring2, &color1, &color2, 0, 0, &count, 0, 0, 0);
    // igraph_count_subisomorphisms_vf2(&ring1, &ring2, NULL, NULL, 0, 0, &count, 0, 0, 0);

    if (count != 21) {
        fprintf(stderr, "Count with two colors failed, expected 21, got %" IGRAPH_PRId ".\n", count);
        return 3;
    }

    igraph_vector_int_destroy(&color1);
    igraph_vector_int_destroy(&color2);

    igraph_destroy(&ring1);
    igraph_destroy(&ring2);

	fclose(fp1);
	fclose(fp2);

    return 0;
}
