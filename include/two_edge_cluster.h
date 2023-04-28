#ifndef TWO_EDGE_CLUSTER
#define TWO_EDGE_CLUSTER

#include "edge.h"
#include <cassert>
#include <tuple>
#include <iostream>
#include <variant>
#include <vector>
#include <bitset>

namespace Splay {
    #include "top_tree.h"
}



using std::vector;
using std::cout;
using std::endl;
using std::bitset;


struct VertexLabel;
struct TwoEdgeConnectivty;
struct TwoEdgeCluster;

using Node = Splay::Node<TwoEdgeCluster,NewEdge,VertexLabel>;
using TwoEdgeTree = Splay::TopTree<TwoEdgeCluster,NewEdge,VertexLabel>;

class TwoEdgeCluster : public Node {
    friend class TwoEdgeConnectivity;

    //Cover level
    inline static int l_max; //floor(log (tree_size))
    int cover_level;
    int cover_plus;
    int cover_minus;
    int global_cover;

    int uncover_val = -1;
    int cover_val = -1;

    NewEdge* min_path_edge;
    NewEdge* min_global_edge;

    void merge_cover(TwoEdgeCluster*, TwoEdgeCluster*);
    void create_cover(NewEdge*, VertexLabel*, VertexLabel*);
    void split_cover(TwoEdgeCluster*, TwoEdgeCluster*);
    void destroy_cover(NewEdge*, VertexLabel*, VertexLabel*);

    //Find Size
    vector<int> size;
    vector<vector<int>> part_size[2]; // could be binary tree

    void compute_part_size(vector<vector<int>>&, vector<vector<int>>&, vector<vector<int>>&, int);
    void sum_row_range(vector<int>&, vector<vector<int>>&, int, int);
    void delete_row_range(vector<vector<int>>&, int, int);
    void sum_diagonal(vector<int>&, vector<vector<int>>&);

    void merge_find_size(TwoEdgeCluster*, TwoEdgeCluster*);
    void create_find_size(NewEdge*, VertexLabel*, VertexLabel*);
    void split_find_size(TwoEdgeCluster*, TwoEdgeCluster*);
    
    
    
    // Find First Label
    VertexLabel* vertex[2] = {nullptr,nullptr};
    int boundary_vertices_id[2] = {-1,-1};
    long int incident;
    vector<long int> part_incident[2];

    //VertexLabel* find_vertex_label(TwoEdgeCluster*, int v, int w, int i);


    void merge_find_first_label(TwoEdgeCluster*, TwoEdgeCluster*);
    void create_find_first_label(NewEdge*, VertexLabel*, VertexLabel*);
    void split_find_first_label(TwoEdgeCluster*, TwoEdgeCluster*);



    public:
    VertexLabel* find_first_label(int, int , int);
    TwoEdgeCluster();
    ~TwoEdgeCluster() {
    };

    static void set_l_max(int);
    static int get_l_max();
    int get_cover_level();

    void cover(int);
    void uncover(int);

    void assign_vertex(int, VertexLabel*);

    int get_size(int);
    long int get_incident();

    void create(NewEdge*, VertexLabel*, VertexLabel*);
    void merge(TwoEdgeCluster*, TwoEdgeCluster*);
    void split(TwoEdgeCluster*, TwoEdgeCluster*);
    void destroy(NewEdge*, VertexLabel*, VertexLabel*);
    void swap_data();


    void print_data() {
        cout << "incident: " << bitset<4>(this->incident) << " ";
        cout << "bounds: (" << this->boundary_vertices_id[0] << "," << this->boundary_vertices_id[1] << ") ";
        cout << " c: " << this->cover_level << " c+: " << this->cover_plus << " c-: " << this->cover_minus << " ";
    }
};


struct VertexLabel {
    vector<vector<NewEdge*>> labels;
    TwoEdgeCluster* leaf_node = nullptr; 

    VertexLabel() {
        int lmax = TwoEdgeCluster::get_l_max();
        this->labels = std::vector<vector<NewEdge*>>(lmax);
        for (int i = 0; i < this->labels.size(); i++) {
            this->labels[i] = std::vector<NewEdge*>();
        }
    };

    ~VertexLabel() {
        /*for (int i = 0; i < labels.size(); i++) {
            for (int j = 0; j < labels[i].size(); j++) {
                delete labels[i][j];
            }
        }*/
    };

};


#endif