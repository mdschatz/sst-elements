// Written by Courtenay T. Vaughan - cleaned of extras - version 7/8/23
#include <iostream>
#include <random>
#include <algorithm>
#include "lutArrays.h"

#ifdef LOG
#include <chrono>
#endif

int main(int argc, char *argv[]) {
  uint64_t i, j;
  uint64_t tmp, src, dst, tct;
  int k, l, a, at, al, b, bt, bl;

  if (argc != 2) {printf("Usage: <fname>\n"); return 1;}

  uint64_t num_indices = indexSize;
  const uint64_t *index = g_pIndex;
  uint64_t edge = edgeSize;
  const uint64_t *edges = g_pEdges;

  uint64_t num_vertices = num_indices - 1;

#ifdef LOG
  printf("num_verticies %lu\n", num_vertices);
  printf("num_edges %lu\n",  edge);
  // Timer values

//  uint64_t *vertex_times = new uint64_t[num_vertices];
//  uint64_t *task_times = new uint64_t[edgeSize];
  uint64_t task_num = 0;
  std::chrono::high_resolution_clock::time_point start_vertex, stop_vertex;
//  std::chrono::high_resolution_clock::time_point start_task, stop_task;
#endif

#ifdef LOG
  start_vertex = std::chrono::high_resolution_clock::now();
//  printf("starting loop\n");
//  fflush(stdout);
#endif
  // Count number of triangles
  tct = 0;
  for (i = 0; i < num_vertices; i++) {
#ifdef LOG
//    printf("Vertex: %d\n", i);
//    fflush(stdout);
#endif

    // going over all of the vertices: i is the vertex number
    at = index[i];
    al = index[i+1];

    if (al > edge) {
      printf("WARNING: Detected malformed input: Index out of edge range\n");
      printf("         Vertex: %u, Edges: %u Index range: [%u,%u)\n",
              i, edge, at, al);
    }

    // at is the pointer into the edges array for i
    for (j = at; j < al && j < edge; j++) {
#ifdef LOG
//      start_task = std::chrono::high_resolution_clock::now();
#endif
      // Begin TASK
      // j is the pointer in the edges array for b, which is second vertex
      bt = index[edges[j]]; bl = index[edges[j]+1];

      // now search through a edges for match with b edges, a triangle
      a = j + 1; // Start search at next possible third vertex
      b = bt;
      while(a < al && b < bl) {
        if (edges[a] < edges[b]) {
          a++;
        } else if (edges[a] > edges[b]) {
          b++;
        } else {
          // printf("triangle at: (i, j, k)=(%d,%d,%d)\n", i, edges[j], edges[a]);
          a++; b++; tct++;
        }
      }
      // End TASK
#ifdef LOG
//      stop_task = std::chrono::high_resolution_clock::now();
//      task_times[task_num++] = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_task - start_task).count();
#endif
    }
#ifdef LOG
//    stop_vertex = std::chrono::high_resolution_clock::now();
//    vertex_times[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_vertex - start_vertex).count();
#endif
  }
#ifdef LOG
  stop_vertex = std::chrono::high_resolution_clock::now();
#endif
#ifdef LOG
  printf("total triangles %lu\n", tct);

//  printf("%u vertex times (ns):", num_vertices);
//  for(i = 0; i < num_vertices; i++) {
//    printf(" %u", vertex_times[i]);
//  }
//  printf("\n");
//  uint64_t total_time = 0;
//  for(i = 0; i < task_num; i++) {
//    total_time += task_times[i];
//  }
  uint64_t kernel_time = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_vertex - start_vertex).count();
  printf("Average task time (%u tasks): %.6f ns\n", edge, 1.0 * kernel_time / edge);

//  delete[] vertex_times;
//  delete[] task_times;
#endif
}
