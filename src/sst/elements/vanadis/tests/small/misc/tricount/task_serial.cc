// Written by Courtenay T. Vaughan - cleaned of extras - version 7/8/23
#include <iostream>
#include <random>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include "lutArrays.h"
#include "taskUtil.h"

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
  tct = doCountTriangles(i,first_i,first_j,last_j,index,edges);
  gettimeofday(&tv2, NULL);
  tv1.tv_sec = tv2.tv_sec - tv1.tv_sec;
  tv1.tv_usec = tv2.tv_usec - tv1.tv_usec;
  printf("total triangles %llu,%u,%u\n", tct, tv1.tv_sec, tv1.tv_usec);
}
