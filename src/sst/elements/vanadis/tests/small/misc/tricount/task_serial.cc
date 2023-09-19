// Written by Courtenay T. Vaughan - cleaned of extras - version 7/8/23
#include <iostream>
#include <random>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

/* Diff from current: a subset of j vertices rather than just one
*/
uint64_t doTask(uint64_t i, uint64_t first_i, uint64_t first_j, uint64_t last_j, uint64_t* index, uint64_t* edges) {
    uint64_t tct = 0;
    
    uint64_t* edges_i = &edges[index[first_i]]; // Start index for the edges i
    uint64_t* edges_j = &edges[index[first_j]]; // Start index for the edges j

    uint64_t A = first_i + i;
    uint64_t* first_A = edges_i + index[A];
    uint64_t* last_A = edges_i + index[A+1];

    first_A = std::lower_bound(first_A, last_A, first_j); // first neighbor of A in paritition j
    if (first_A == last_A) return 0;

    for ( ; first_A < last_A; first_A++) {
        uint64_t B = *first_A;
        if (B >= last_j) break; // all the rest of the neighbors are outside partition j
        
        uint64_t * this_A = first_A + 1;
        uint64_t * this_B = edges_j + index[B];
        uint64_t * last_B = edges_j + index[B+1];

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

    /*
uint64_t al, ae, at, a, b, bl;

  //asm volatile ("rdcycle %0" : "=r" (cycles));
  // going over all of the vertices: i is the vertex number
  tct = 0; // Triangle count
  al = index[index_i+1]; ae = al - 1;
  at = index[index_i];
  // at is the pointer into the edges array for i
  b = index[edges[index_j]]; bl = index[edges[index_j]+1];
  a = index_j + 1;
  
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
  //asm volatile ("rdcycle %0" : "=r" (cycles));

    return tct;*/
}
int main(int argc, char *argv[]) {
  uint64_t tct;

  if (argc != 6) {printf("Usage: <graphname> <first_i> <first_j> <last_j> <i>\n"); return 1;}

  std::string fn = argv[1];
  unsigned int cycles;
  //static FILE* fp; // should work now, there was a bug in Vanadis
  //fp = fopen(fn.c_str(), "r");
  int fp = open( fn.c_str(), O_RDONLY|O_CREAT, S_IRWXU );
  if (!fp) {
      printf("%s() open '%s' failed\n",__func__, fn);
      exit(-1);
  }
  uint64_t first_i = std::stoll(argv[2]);
  uint64_t first_j = std::stoll(argv[3]);
  uint64_t last_j = std::stoll(argv[4]);
  uint64_t i = std::stoll(argv[5]);
  uint64_t num_vertices;
  uint64_t num_edges;
  
  read(fp, &num_vertices, sizeof(num_vertices));
  read(fp, &num_edges, sizeof(num_edges));
  
  uint64_t* index = new uint64_t[num_vertices+1];
  uint64_t *edges = new uint64_t[num_edges];

  read(fp, index, sizeof(uint64_t)*(num_vertices+1));
  read(fp, edges, sizeof(uint64_t)*num_edges);
  //fclose(fp);
  close(fp); 
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
