#include <catch2/catch_test_macros.hpp>
#include "top_tree.h"
#include <climits>

struct AddWeightCluster : Node<AddWeightCluster, int, None> {
    int max_weight;
    int extra;

    void create(int* edge_data, None* left, None* right) {
        this->extra = 0;
        if (this->is_path()) {
            max_weight = *edge_data;
        } else {
            max_weight = INT_MIN;
        }
    };

    void split_leaf(int* edge_data, None* left, None* right) {
        *edge_data += this->extra;
    };

    void merge(AddWeightCluster* left, AddWeightCluster* right) {
        this->max_weight = std::max(left->max_weight, right->max_weight);
        
        this->max_weight = std::max(
            left->is_path() ? left->max_weight : INT_MIN, 
            right->is_path() ? right->max_weight : INT_MIN
        );
        this->extra = 0;
    };

    void split(AddWeightCluster* left, AddWeightCluster* right) {
        if (left->is_path()) {
            left->max_weight += this->extra;
            left->extra += this->extra;
        }
        if (right->is_path()) {
            right->max_weight += this->extra;
            right->extra += this->extra;
        }
    };

    void add_weight(int weight) {
        max_weight += weight;
        extra += weight;
    }
    void print_data() {
        std::cout << "weight: " << this->max_weight << " num_bound: " << this->get_num_boundary_vertices() << " ";
    }
};


TEST_CASE("Add weight full example", "[user data]") {
    int size = 10;
    TopTree<AddWeightCluster, int, None> top_tree = TopTree<AddWeightCluster, int, None>(size);
    top_tree.link(0, 1, 2);
    top_tree.link(1, 2, 2);
    top_tree.link(1, 3, 3);
    top_tree.link(3, 8, 1);
    top_tree.link(8, 9, 2);
    top_tree.link(3, 4, 5);
    top_tree.link(4, 5, 1);
    top_tree.link(4, 6, 1);
    top_tree.link(6, 7, 1);
    
    AddWeightCluster* root;

    SECTION("Max weight on path") {
        root = top_tree.expose(0, 9);
        REQUIRE(root->max_weight == 3);

        root->add_weight(10);
        REQUIRE(root->max_weight == 13);
        top_tree.deexpose(0, 9);

        root = top_tree.expose(9, 5);
        REQUIRE(root->max_weight == 12);
        top_tree.deexpose(9, 5);

        top_tree.cut(1, 3);
        top_tree.link(0, 5, 13);

        root = top_tree.expose(9, 7);
        REQUIRE(root->max_weight == 12);
        top_tree.deexpose(9, 7);

        root = top_tree.expose(9, 0);
        REQUIRE(root->max_weight == 13);
        root->add_weight(1);
        top_tree.deexpose(9, 0);

        root = top_tree.expose(3, 7);   
        REQUIRE(root->max_weight == 6);
        top_tree.deexpose(7, 3);
        
        top_tree.cut(4, 3);
        root = top_tree.expose(5, 7);   
        REQUIRE(root->max_weight == 2);
        top_tree.deexpose(7, 5);
        
        root = top_tree.expose(3, 9);   
        REQUIRE(root->max_weight == 13);
        top_tree.deexpose(9, 3);
        
        top_tree.link(9, 7, 3);
        root = top_tree.expose(2, 3);   
        REQUIRE(root->max_weight == 14);
        top_tree.deexpose(2, 3);

    }   
}




