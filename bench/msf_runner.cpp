#include <min_spanning_forest.h>
#include <MST.h>

#include <stdio.h>
#include <fstream>
#include <cassert>
#include <chrono>


struct MSF_Query {
    int u;
    int v;
    int weight;
};

int main(int argc, char *argv[]) {
    
    //Parse text-file as data
    std::ifstream data_file(argv[1]);

    std::string name;

    std::getline(data_file,name);

    std::string problem;
    int n;
    int m;
    data_file >> problem >> n >> m;

    std::vector<MSF_Query> queries;

    for (int i = 0; i < m; i++) {
        char query;
        int u;
        int v;
        int weight;
        data_file >> query >> u >> v >> weight;
        assert(query == 'e');
        queries.push_back(MSF_Query{u,v,weight});
    }

    //Start clock
    auto start = std::chrono::high_resolution_clock::now();

    //Run measurement
    MinSpanForest tree(n);
    for (MSF_Query q : queries) {
        tree.insert(q.u,q.v,q.weight);
    }
    
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> time_taken = end - start;

    //Return elapsed time
    std::cout << time_taken.count() << "\n";
    return 0;
}

