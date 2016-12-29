// File: RPCServer.cpp --------------------------------------------------------
//	Assignment 3
//	Course: CPSC 526
//	Author: Xi Wang
//	Time: Nov 29, 2016
// ----------------------------------------------------------------------------

#include "RPCServer.hpp"

Status RPCServer::AddNode(ServerContext* context, const Node* node, Reply* reply) {
	u64 node_id = node->node_id();
	GraphServer::graph.add_node(node_id);
	reply->set_message("Add node " + to_string(node_id) + " successfully!");
	return Status::OK;
}
Status RPCServer::AddEdge(ServerContext* context, const Edge* edge, Reply* reply) {
	u64 node_a_id = edge->node_a_id();
	u64 node_b_id = edge->node_b_id();
	GraphServer::graph.add_edge(node_a_id, node_b_id);
	reply->set_message("Add edge " + to_string(node_a_id) + "<->" + to_string(node_b_id) + " successfully!");
	return Status::OK;
}
Status RPCServer::RemoveNode(ServerContext* context, const Node* node, Reply* reply) {
	u64 node_id = node->node_id();
	GraphServer::graph.remove_node(node_id);
	reply->set_message("Remove node " + to_string(node_id) + " successfully!");
	return Status::OK;
}
Status RPCServer::RemoveEdge(ServerContext* context, const Edge* edge, Reply* reply) {
	u64 node_a_id = edge->node_a_id();
	u64 node_b_id = edge->node_b_id();
	GraphServer::graph.remove_edge(node_a_id, node_b_id);
	reply->set_message("Remove edge " + to_string(node_a_id) + "<->" + to_string(node_b_id) + " successfully!");
	return Status::OK;
}