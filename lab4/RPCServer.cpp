// File: RPCServer.cpp --------------------------------------------------------
//	Assignment 3
//	Course: CPSC 526
//	Author: Xi Wang
//	Time: Nov 29, 2016
// ----------------------------------------------------------------------------

#include "RPCServer.hpp"

Status RPCServer::GetNode(ServerContext* context, const Node* node, Reply* reply) {
	pthread_mutex_lock (&mtx);
	u64 node_id = node->node_id();
	if (GraphServer::graph.get_node(node_id)) {
		reply->set_message(200);
	} else {
		reply->set_message(400);
	}
	pthread_mutex_unlock (&mtx);
	return Status::OK;
}

Status RPCServer::AddNode(ServerContext* context, const Node* node, Reply* reply) {
	pthread_mutex_lock (&mtx);
	u64 node_id = node->node_id();
	if (GraphServer::graph.get_node(node_id)) {
		reply->set_message(204);
	} else {
		GraphServer::graph.add_node(node_id);
		reply->set_message(200);
	}
	pthread_mutex_unlock (&mtx);
	return Status::OK;
}

Status RPCServer::AddEdge(ServerContext* context, const Edge* edge, Reply* reply) {
	pthread_mutex_lock (&mtx);
	u64 node_a_id = edge->node_a_id();
	u64 node_b_id = edge->node_b_id();
	if (GraphServer::graph.get_edge(node_a_id, node_b_id)) {
		reply->set_message(204);
	} else if (GraphServer::graph.get_node(node_a_id)) {
	    GraphServer::graph.add_edge(node_a_id, node_b_id);
		reply->set_message(200);
  	} else {
		reply->set_message(400);
	}
	pthread_mutex_unlock (&mtx);
	return Status::OK;
}
// not called
Status RPCServer::RemoveNode(ServerContext* context, const Node* node, Reply* reply) {
	u64 node_id = node->node_id();
	GraphServer::graph.remove_node(node_id);
	reply->set_message(200);
	return Status::OK;
}
Status RPCServer::RemoveEdge(ServerContext* context, const Edge* edge, Reply* reply) {
	pthread_mutex_lock (&mtx);
	u64 node_a_id = edge->node_a_id();
	u64 node_b_id = edge->node_b_id();
	if (GraphServer::graph.get_edge(node_a_id, node_b_id)) {
		GraphServer::graph.remove_edge(node_a_id, node_b_id);
		reply->set_message(200);
	} else {
	    reply->set_message(400);
	}
	pthread_mutex_unlock (&mtx);
	return Status::OK;
}
