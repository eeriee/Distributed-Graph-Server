//  File: Graph.hpp ------------------------------------------------------------
//  Assignment 2
//  Course: CPSC 526
//  Author: Xi Wang
//  Time: Oct 22, 2016
// ----------------------------------------------------------------------------

#include "tools.hpp"

class Graph {
    private:
        map<u64, set<u64>> adj_list;

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
