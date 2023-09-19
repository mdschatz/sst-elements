// Written by Courtenay T. Vaughan - cleaned of extras - version 7/8/23
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>
#include <vector>

int main(int argc, char *argv[]) {
  uint64_t i, j;
  uint64_t tmp, src, dst, edge = 0, tct;
  int k, l, a, at, al, b, bt, bl;

  if (argc != 9) {printf("Usage: <seed> <scale> <edge ratio> <A> <B> <C> <NL> <fname>\n"); return 1;}

  uint64_t seed = std::stoll(argv[1]);
  uint64_t scale = std::stoll(argv[2]);
  uint64_t num_vertices = 1 << std::stoll(argv[2]);
  uint64_t num_edges = num_vertices * std::stoll(argv[3]);

  double A = std::stod(argv[4]) / 100.0;
  double B = std::stod(argv[5]) / 100.0;
  double C = std::stod(argv[6]) / 100.0;
  double D = 1.0 - A - B - C;

  uint64_t NL = std::stoll(argv[7]);
  double quad = ( -1.0 + std::sqrt(1.0 + 4.0 * 12.0 * NL) ) / 2.0;
  uint64_t NT = std::llround(quad);

  std::string filename(argv[8]);

  std::mt19937_64 gen64(seed + num_edges);
  uint64_t max_rand = gen64.max();

  uint64_t *index = new uint64_t[num_vertices+1];
  std::fill(index, index + num_vertices + 1, 0);

  uint64_t *edges = new uint64_t[num_edges];
  for (i = 0; i < num_edges; i++) {

    // Generate new edge src and dst ids
    src = dst = 0;
    for (j = 0; j < scale; j++) {
      double rand = (double) gen64() / (double) max_rand;

      src <<= 1;
      if (rand > A + B) { src += 1; }

      dst <<= 1;
      if (rand > A && rand <= A + B || rand > A + B + C) { dst += 1; }
    }

    if (src == dst) continue;  // do not include self edges

    // Edge inclusion
    // Order edge definition
    if (src > dst) {
      tmp = dst; dst = src; src = tmp;
    }

    // Determine whether edge list update is needed
    // index[src] - index[src+1] is the edge index range for the src vertex
    tmp = 1;
    for (k = index[src]; k < index[src+1]; k++) {
      if (edges[k] >= dst) {
        if (edges[k] == dst) {
          tmp = 0;
        }
        break;
      }
    }

    // Insert the new edge into the list and update structures
    // Find location for insertion (dsts are ordered by vertex id)
    if (tmp) {
      if (index[num_vertices] > 0)
        for (l = index[num_vertices]; l >= k; l--)
          edges[l+1] = edges[l];
      edges[k] = dst;
      for (k = num_vertices; k > src; k--)
        index[k]++;
      edge++;
    }
  }

  printf("num_verticies %d\n", num_vertices);
  printf("num_edges %d\n",  edge);

  // Define rank tasks
  // Algorithm adapted from SHAD reference implementation
  uint64_t incr = (edge * 1.5) / NT;
  uint64_t edge_target = incr;

  uint64_t vertex=0;
  uint64_t task = 1;
  std::vector<uint64_t> tasks;
  tasks.push_back(vertex);
  for (uint64_t vertex = 0; vertex < num_vertices; ++vertex) {  // for each vertex

    uint64_t vertex_start = index[vertex];

    if (vertex_start < edge_target) continue;               // ... continue until edge target is reached
    if (vertex > 0) tasks.push_back(vertex);               // ... else store start vertex for this task

    uint64_t task_size = incr + (vertex_start - edge_target);

    if (edge_target == edge) break;                 // ... all done

    // incr reduces at 1/4 NT and 3/4 NT
    if      ( task < std::llround(0.25 * NT) ) incr = (edge * 1.5) / NT;
    else if ( task < std::llround(0.75 * NT) ) incr = (edge * 1.0) / NT;
    else                                       incr = (edge * 0.5) / NT;

    ++task;
    edge_target = std::min(edge, vertex_start + incr);
  }
  tasks.push_back(num_vertices);

  // From https://stackoverflow.com/questions/60442391/c-how-to-store-large-binary-lookup-table-with-application/70850435#70850435
  std::string headerName = filename + ".h";
  std::ofstream hfile(headerName, std::ios::out);
  hfile << "#include \"stdint.h\"" << std::endl;
  hfile << "enum {" << std::endl;
  hfile << "  indexSize = " << num_vertices + 1 << "," << std::endl;
  hfile << "  edgeSize = " << edge << "," << std::endl;
  hfile << "  taskSize = " << tasks.size() << "," << std::endl;
  hfile << "};" << std::endl;
  hfile << "extern const uint64_t* g_pIndex;" << std::endl;
  hfile << "extern const uint64_t* g_pEdges;" << std::endl;
  hfile << "extern const uint64_t* g_pTasks;" << std::endl;
  hfile.close();

  std::string ccName = filename + ".cc";
  std::ofstream ccFile(ccName, std::ios::out);
  ccFile << "#include \"lutArrays.h\"" << std::endl << std::endl;
  ccFile << "const uint64_t g_index[indexSize] = {";
  for (i = 0; i < num_vertices; i++) {
    ccFile << index[i] << ", ";
  }
  ccFile << index[num_vertices] << "};" << std::endl;

  ccFile << "const uint64_t g_edges[edgeSize] = {";
  for (i = 0; i < edge - 1; i++) {
    ccFile << edges[i] << ", ";
  }
  ccFile << edges[edge - 1] << "};" << std::endl << std::endl;

  ccFile << "const uint64_t g_tasks[taskSize] = {";
  for (i = 0; i < tasks.size() - 1; i++) {
    ccFile << tasks[i] << ", ";
  }
  ccFile << tasks[tasks.size() - 1] << "};" << std::endl << std::endl;

  ccFile << "const uint64_t *g_pIndex = g_index;" << std::endl;
  ccFile << "const uint64_t *g_pEdges = g_edges;" << std::endl;
  ccFile << "const uint64_t *g_pTasks = g_tasks;" << std::endl;
  ccFile.close();

  delete[] index;
  delete[] edges;
}
