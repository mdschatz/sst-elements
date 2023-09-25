// Written by Courtenay T. Vaughan - cleaned of extras - version 7/8/23
#include <algorithm>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <random>
#include <unistd.h>
#include <fcntl.h>

#include "lutArrays.h"

using namespace std;

uint64_t doCountTriangles(uint64_t i, uint64_t first_i, uint64_t first_j, uint64_t last_j, const uint64_t* index, const uint64_t* edges) {
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
              // (Parallel) Debug printing
              // omp_set_lock( &lock );
              // printf("[%u] triangle at: (i, j, k)=(%d,%d,%d)\n", tid, first_i + i, B, *this_A);
              // omp_unset_lock( &lock );
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
    uint64_t a, al, ae, at, b, bl;

    if (argc != 1) {printf("Usage: <>\n"); return 1;}

    omp_lock_t lock;
    omp_init_lock(&lock);

    uint64_t num_indices = indexSize;
    const uint64_t *index = g_pIndex;
    uint64_t num_edges = edgeSize;
    const uint64_t *edges = g_pEdges;
    uint64_t num_tasks = taskSize;
    const uint64_t *tasks = g_pTasks;

    uint64_t num_vertices = num_indices - 1;
    num_tasks -= 1; // Adjust for data structure

    printf("num_verticies %d\n", num_vertices);
    printf("num_edges %d\n",  num_edges);
    printf("num_tasks %d\n",  num_tasks);
    fflush(stdout);
    
    #pragma omp parallel reduction(+ : tct)
    {
    
    // Whereami
    const uint64_t tid = (uint64_t)omp_get_thread_num();
    const uint64_t nthreads = (uint64_t)omp_get_num_threads();
    const int64_t threads_per_rank = 8; // Corresponds to the number of Cores
    const uint64_t nranks = nthreads / threads_per_rank;

    // Compute offsets into the two partitions I & J
    uint64_t tid_off_I = tid / threads_per_rank;
    uint64_t tid_off_J = tid % threads_per_rank;

    // Determine the amount of work in the I & J partitions
    // Done by balancing balance partition indices evenly
    const uint64_t work_I = (num_tasks - tid_off_I - 1) / nranks + 1;
    const uint64_t work_J = (num_tasks - tid_off_J - 1) / threads_per_rank + 1;

    // Convert tid_off into task_off
    uint64_t task_off_I = tid_off_I * (num_tasks / nranks) + std::min(num_tasks % nranks, tid_off_I);
    uint64_t task_off_J = tid_off_J * (num_tasks / threads_per_rank) + std::min(num_tasks % threads_per_rank, tid_off_J);

    // omp_set_lock( &lock );
    // printf("tid: [%u/%u] Starting count: num_tasks: %u tid_off (I,J): (%u,%u) task: (%u,%u) work (I,J): (%u,%u)\n",
    //        tid, nthreads,
    //        num_tasks,
    //        tid_off_I, tid_off_J,
    //        task_off_I, task_off_J,
    //        work_I, work_J);
    // omp_unset_lock( &lock );

    // Iterate over partition I indices
    tct = 0; // Triangle count
    for (size_t i = task_off_I; i < task_off_I + work_I && i < num_tasks; i++) {
      uint64_t vertices_i = tasks[i];
      uint64_t vertices_ip1 = tasks[i+1];

      // Iterate over partition J indices
      for (size_t j = task_off_J; j < task_off_J + work_J && j < num_tasks; j++) {
        uint64_t vertices_j = tasks[j];
        uint64_t vertices_jp1 = tasks[j+1];

        // Find triangles one vertex at a time
        for (size_t xi = 0; xi < vertices_ip1 - vertices_i; xi++) {
          tct += doCountTriangles(xi, vertices_i, vertices_j, vertices_jp1, index, edges);
        }
      }

      // omp_set_lock( &lock );
      // printf("[%u/%u] total triangles %ld\n", tid, nthreads, tct);
      // omp_unset_lock( &lock );
    }
    } // End parallel
    printf("total triangles %ld\n", tct);
}
