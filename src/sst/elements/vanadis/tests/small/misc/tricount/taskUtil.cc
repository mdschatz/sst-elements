#include "taskUtil.h"

/* Diff from current: a subset of j vertices rather than just one
*/
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

