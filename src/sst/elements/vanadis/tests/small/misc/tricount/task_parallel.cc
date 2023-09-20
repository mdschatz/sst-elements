// Written by Courtenay T. Vaughan - cleaned of extras - version 7/8/23
#include <omp.h>
#include <iostream>
#include <fstream>
#include <random>
#include <unistd.h>
#include <fcntl.h>

#include "lutArrays.h"

using namespace std;
int main(int argc, char *argv[]) {
    uint64_t tct;
    uint64_t a, al, ae, at, b, bl;

    if (argc != 1) {printf("Usage: <>\n"); return 1;}

    uint64_t num_indices = indexSize;
    const uint64_t *index = g_pIndex;
    uint64_t num_edges = edgeSize;
    const uint64_t *edges = g_pEdges;
    uint64_t num_tasks = taskSize;
    const uint64_t *tasks = g_pTasks;

    uint64_t num_vertices = num_indices - 1;

    printf("num_verticies %d\n", num_vertices);
    printf("num_edges %d\n",  num_edges);
    printf("num_tasks %d\n",  num_tasks);
    fflush(stdout);
    
// Leave tct private for now, maybe atomic later once this works correctly
#pragma omp parallel private(tct) 
    {
    // going over all of the vertices: i is the vertex number
    
    uint32_t tid = omp_get_thread_num();
    uint32_t nthreads = omp_get_num_threads();

    const uint64_t tasks_per_tid = (num_tasks - 1) / nthreads + 1;
    const uint64_t tStart = std::min(num_tasks, tid * tasks_per_tid);
    const uint64_t tStop = std::min(num_tasks, (tid+1)*tasks_per_tid);

    tct = 0; // Triangle count
    printf("tid: [%u/%u] Starting count\n", tid, nthreads);
    for (size_t i = tStart; i < tStop; i++) {
        uint64_t index_i = tasks[i];
        uint64_t index_j = tasks[i+1];

        al = index[index_i+1]; ae = al - 1;
        at = index[index_i];
        // at is the pointer into the edges array for i
        b = index[edges[index_j]]; bl = index[edges[index_j]+1];
        a = index_j + 1;
        printf("%u, task %llu,%llu\n",tid,index_i,index_j);
        // now search through a edges for match with b edges, a triangle
        while(a < al && b < bl) {
            if (edges[a] < edges[b]) {
                a++;
            } else if (edges[a] > edges[b]) {
                b++;
            } else {
                a++; b++; tct++;
            }
        }
        printf("total triangles %lu %ld\n", tid, tct);
    }
    } // End parallel
}
