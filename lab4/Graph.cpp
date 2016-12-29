//  File: Graph.hpp -----------------------------------------------------------
//  Assignment 4
//  Course: CPSC 526
//  Author: Xi Wang
//  Time: Dec 19, 2016
// ----------------------------------------------------------------------------
#include "Graph.hpp"


u64 Graph::get_node_num() {
    return adj_list.size();
}

bool Graph::get_node(u64 node_id) {
    if (adj_list.count(node_id)) {
        return true;
    }
    return false;
}

bool Graph::get_edge(u64 node_a_id, u64 node_b_id) {
    if (adj_list.count(node_a_id) && adj_list[node_a_id].count(node_b_id)) {
        return true;
    }
    return false;
}

set<u64> Graph::get_neighbors(u64 node_id) {
    if(get_node(node_id))
        return adj_list[node_id];
    return {};
}

void Graph::add_node(u64 node_id) {
    set<u64> neighbors;
    adj_list[node_id] = neighbors;
}

//if both nodes exist in the partition, need to call add_edge twice with opposite id orders
void Graph::add_edge(u64 node_a_id, u64 node_b_id) {
    if(get_node(node_a_id)){
        adj_list[node_a_id].insert(node_b_id);
    }
}

//no call to remove_node in partition
void Graph::remove_node(u64 node_id) { 
    if(!get_node(node_id)) return;
    set<u64> neighbors = adj_list[node_id];
    for (u64 ne: neighbors) {
        adj_list[ne].erase(node_id);
    }
    adj_list.erase(node_id);
}

//if both nodes exist in the partition, need to call remove_edge twice with opposite id orders
void Graph::remove_edge(u64 node_a_id, u64 node_b_id) {
    if(get_edge(node_a_id, node_b_id)){
        adj_list[node_a_id].erase(node_b_id);
    }
}

//no call to shortest_path in partition
u64 Graph::shortest_path(u64 node_a_id, u64 node_b_id){
    if(!get_node(node_a_id) || !get_node(node_b_id)) return 0;

    unordered_map<u64, bool> visited;
    unordered_map<u64, u64> dist;

    for(auto adj: adj_list){
        dist[adj.first] = UINT64_MAX;
    }
    dist[node_a_id] = 0;
    priority_queue<pair<u64, u64>, vector<pair<u64, u64>>, greater<pair<int, int>>> pq;
    pq.push(make_pair(0, node_a_id));

    while(!pq.empty()){
        u64 v = pq.top().second;
        pq.pop();
        if(v == node_b_id) return dist[v];
        if(visited[v]) continue;
        visited[v] = true;

        for(u64 u: adj_list[v]){
            if(!visited[u] && dist[u] > dist[v] + 1){
                dist[u] = dist[v] + 1;
                pq.push(make_pair(dist[u], u));
            }
        }
    }
    return 0;
}

void Graph::display() {
    cout << "Graph Display Start -----------------------------------------" << endl;
    for (auto it: adj_list) {
        cout << it.first << ": ";
        for (u64 node: it.second) {
            cout << node << ' ';
        }
        cout << endl;
    }
    cout << "Graph Display end -------------------------------------------" << endl;
}
