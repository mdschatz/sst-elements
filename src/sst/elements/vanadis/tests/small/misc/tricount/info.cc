// Written by Courtenay T. Vaughan - cleaned of extras - version 7/8/23
#include <iostream>
#include <random>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <vector>

/* Diff from current: a subset of j vertices rather than just one
*/
uint64_t doTask(uint64_t i, uint64_t first_i, uint64_t first_j, uint64_t last_j, uint64_t* index, uint64_t* edges) {
    uint64_t cost = 0;
    
    uint64_t* edges_i = &edges[index[first_i]];
    uint64_t* edges_j = &edges[index[first_j]];

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
        
        uint64_t costA = last_A - this_A;
        uint64_t costB = last_B - this_B;
        if ((costA != 0) && (costB != 0))
            cost += costA + costB;
    }
    return cost;
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

  if (argc != 3) {printf("Usage: <graphname> <NT>\n"); return 1;}

  std::string fn = argv[1];
  unsigned int cycles;
  //static FILE* fp; // should work now, there was a bug in Vanadis
  //fp = fopen(fn.c_str(), "r");
  int fp = open( fn.c_str(), O_RDONLY|O_CREAT, S_IRWXU );
  if (!fp) {
      printf("%s() open '%s' failed\n",__func__, fn);
      exit(-1);
  }
  uint64_t NL = std::stoll(argv[2]);
  double quad = ( -1.0 + std::sqrt(1.0 + 4.0 * 12.0 * NL) ) / 2.0;
  uint64_t NT = std::llround(quad);
  uint64_t num_vertices;
  uint64_t num_edges;
  
  read(fp, &num_vertices, sizeof(num_vertices));
  read(fp, &num_edges, sizeof(num_edges));
  
  // Cannot read in the full index/edge list and don't need it anyways
  // Do need to spread index & edges out in memory (probably distinct pages is sufficient)
  uint64_t* index = new uint64_t[num_vertices+1];
  uint64_t *edges = new uint64_t[num_edges];

  read(fp, index, sizeof(uint64_t)*(num_vertices+1));
  read(fp, edges, sizeof(uint64_t)*num_edges);
  //fclose(fp);
  close(fp); 
  printf("num_verticies %d\n", num_vertices);
  printf("num_edges %d\n",  num_edges);

  // Algorithm adapted from SHAD reference implementation
  uint64_t task = 1;
  uint64_t incr = (1.5 * num_edges) / NT;
  uint64_t edge_target = incr;

  uint64_t vertex=0;
  std::vector<uint64_t> tasks;
  tasks.push_back(vertex);
  for (uint64_t vertex = 0; vertex < num_vertices; ++vertex) {  // for each vertex

    int vertex_start;
    vertex_start = index[vertex];

    if (vertex_start < edge_target) continue;               // ... continue until edge target is reached
    if (vertex > 0) tasks.push_back(vertex);               // ... else store start vertex for this task

    uint64_t task_size = incr + (vertex_start - edge_target);

    if (edge_target == num_edges) break;                 // ... all done

    // incr reduces at 1/4 NT and 3/4 NT
    if      ( task < std::llround(0.25 * NT) ) incr = (1.5 * num_edges) / NT;
    else if ( task < std::llround(0.75 * NT) ) incr = (1.0 * num_edges) / NT;
    else                                       incr = (0.5 * num_edges) / NT;

    ++task;
    edge_target = (vertex_start + incr) > num_edges ? num_edges : vertex_start + incr;
  }
  tasks.push_back(num_vertices);
  
  printf("Tasks: ");
  for (int i = 0; i < tasks.size(); i++)
  {
      printf("%llu, ", tasks[i]);
  }
  uint64_t first_i;
  uint64_t first_j;
  uint64_t last_j;
  for (size_t i = 0; i < tasks.size() - 1; i++) {
    first_i = tasks[i];
    uint64_t range = tasks[i+1] - tasks[i];
    for (size_t j = i; j < tasks.size() - 1; j++) {
        first_j = tasks[j];
        last_j = tasks[j+1];
        for (uint64_t x = 0; x < range; x++) {
            uint64_t cost = doTask(x,first_i,first_j,last_j,index,edges);
            printf("%llu,%llu,%llu,%llu,%llu\n",first_i,first_j,last_j,x,cost);
        }
    }
  }
}
