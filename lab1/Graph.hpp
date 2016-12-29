// File: Graph.hpp -------------------------------------------------------------
//  Assignment 1
//  Course: CPSC 526
//  Author: Xi Wang
//  Time: Sep 18, 2016
// ----------------------------------------------------------------------------
#include <string>
#include <unordered_map>
#include <set>
#include <climits>
#include <vector>
#include <queue>
#include <stdint.h>
#include <inttypes.h>
#include <iostream>

using namespace std;

typedef uint64_t u64;

class Graph {
    private:
        unordered_map<u64, set<u64>> adj_list;
        u64 node_num;

    public:
        u64 get_node_num();
        bool get_node(u64 node_id);
        bool get_edge(u64 node_a_id, u64 node_b_id);
        set<u64> get_neighbors(u64 node_id);
        void add_node(u64 node_id);
        void add_edge(u64 node_a_id, u64 node_b_id);
        void remove_node(u64 node_id);
        void remove_edge(u64 node_a_id, u64 node_b_id);
        u64 shortest_path(u64 node_a_id, u64 node_b_id);
        void display();

};
