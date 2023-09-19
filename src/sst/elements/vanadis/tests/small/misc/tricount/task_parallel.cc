// Written by Courtenay T. Vaughan - cleaned of extras - version 7/8/23
#include <omp.h>
#include <iostream>
#include <fstream>
#include <random>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
int main(int argc, char *argv[]) {
    uint64_t tct;
    uint64_t a, al, ae, at, b, bl;

    int nthreads, tid;
    char* tmp = getenv("OMP_NUM_THREADS");
    
    if (argc != 3) {printf("Usage: <graphfile> <taskfile>\n"); return 1;}

    std::string fn = argv[1];

    int fp = open( fn.c_str(), O_RDONLY|O_CREAT, S_IRWXU );
    if (!fp) {
        printf("%s() open '%s' failed\n",__func__, fn);
        exit(-1);
    }
    uint64_t num_vertices;
    uint64_t num_edges;
  
    read(fp, &num_vertices, sizeof(num_vertices));
    read(fp, &num_edges, sizeof(num_edges));
  
    // Cannot read in the full index/edge list and don't need it anyways
    // Do need to spread index & edges out in memory (probably distinct pages is sufficient)
    uint64_t *index = new uint64_t[num_vertices+1];
    uint64_t *edges = new uint64_t[num_edges];

    read(fp, index, sizeof(uint64_t)*(num_vertices+1));
    read(fp, edges, sizeof(uint64_t)*num_edges);
    close(fp); 
    printf("num_verticies %d\n", num_vertices);
    printf("num_edges %d\n",  num_edges);
    
    std::vector<std::vector<uint64_t> > tasklist;
    // Read tasks
    fn = argv[2];
    std::ifstream tfile (fn);
    if (tfile.is_open()) {
        std::string taskstr;
        while (getline(tfile,taskstr)) {

        std::vector<uint64_t> vec;
        size_t pos = 0;
        while ((pos = taskstr.find(',')) != std::string::npos) {
            std::string vnum = taskstr.substr(0,pos);
            vec.push_back(std::stoll(vnum));
            taskstr.erase(0, pos + 1);
        }
        vec.push_back(std::stoll(taskstr));
        tasklist.push_back(vec);
        }
        tfile.close();
    } else {
        printf("%s() open '%s' failed\n",__func__, fn);
    }
    
    /*printf("Read taskfile:\n");
    for (int i = 0; i < tasklist.size(); i++) {
        printf("Thread %d: ",i);
        for (int j = 0; j < tasklist[i].size(); j++) {
            printf("%llu,",tasklist[i][j]);
        }
        printf("\n");
    }
    return 0;
*/
    
// Leave tct private for now, maybe atomic later once this works correctly
#pragma omp parallel private(tct) 
    {
    // going over all of the vertices: i is the vertex number
    
    uint32_t tid = omp_get_thread_num();
    tct = 0; // Triangle count
    if (tasklist.size() > tid) {
    for (size_t i = 1; i < tasklist[tid].size(); i += 2) {
        uint64_t index_i = tasklist[tid][i-1];
        uint64_t index_j = tasklist[tid][i];
        //printf("Thread %lu executing task (%llu, %llu)\n", tid, index_i, index_j);
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
    }}
    } // End parallel
}
