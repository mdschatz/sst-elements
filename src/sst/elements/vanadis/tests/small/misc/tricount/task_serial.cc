// Written by Courtenay T. Vaughan - cleaned of extras - version 7/8/23
#include <iostream>
#include <random>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include "lutArrays.h"

/* Diff from current: a subset of j vertices rather than just one
*/
uint64_t doTask(uint64_t i, uint64_t first_i, uint64_t first_j, uint64_t last_j, const uint64_t* index, const uint64_t* edges) {
    uint64_t tct = 0;
    
    const uint64_t* edges_i = &edges[index[first_i]]; // Start index for the edges i
    const uint64_t* edges_j = &edges[index[first_j]]; // Start index for the edges j

    uint64_t A = first_i + i;
    const uint64_t* first_A = edges_i + (index[A] - index[first_i]);
    const uint64_t* last_A = edges_i + (index[A+1] - index[first_i]);

    first_A = std::lower_bound(first_A, last_A, first_j); // first neighbor of A in paritition j
    if (first_A == last_A) return 0;

    for ( ; first_A < last_A; first_A++) {
        uint64_t B = *first_A;
        if (B >= last_j) break; // all the rest of the neighbors are outside partition j
        
        const uint64_t * this_A = first_A + 1;
        const uint64_t * this_B = edges_j + (index[B] - index[first_j]);
        const uint64_t * last_B = edges_j + (index[B+1] - index[first_j]);

        while ((this_A < last_A) && (this_B < last_B)) {
            if (*this_A < *this_B) {
              this_A++;
            } else if (*this_A > *this_B) {
              this_B++;
            } else {
              this_A++;
              this_B++;
              tct++;
            }
        }   
    }
    return tct;
}

int main(int argc, char *argv[]) {
  uint64_t tct;

  if (argc != 5) {printf("Usage: <first_i> <first_j> <last_j> <i>\n"); return 1;}

  unsigned int cycles;
  uint64_t first_i = std::stoll(argv[1]);
  uint64_t first_j = std::stoll(argv[2]);
  uint64_t last_j = std::stoll(argv[3]);
  uint64_t i = std::stoll(argv[4]);

  uint64_t num_indices = indexSize;
  const uint64_t *index = g_pIndex;
  uint64_t num_edges = edgeSize;
  const uint64_t *edges = g_pEdges;
  uint64_t num_tasks = taskSize;
  const uint64_t *tasks = g_pTasks;

  uint64_t num_vertices = num_indices - 1;
  
  printf("num_verticies %d\n", num_vertices);
  printf("num_edges %d\n",  num_edges);

  struct timeval tv1;
  struct timeval tv2;
  gettimeofday(&tv1, NULL);
  tct = doTask(i,first_i,first_j,last_j,index,edges);
  gettimeofday(&tv2, NULL);
  tv1.tv_sec = tv2.tv_sec - tv1.tv_sec;
  tv1.tv_usec = tv2.tv_usec - tv1.tv_usec;
  printf("total triangles %llu,%u,%u\n", tct, tv1.tv_sec, tv1.tv_usec);
}
