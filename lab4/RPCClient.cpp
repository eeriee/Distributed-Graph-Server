// File: RPCClient.cpp --------------------------------------------------------
//	Assignment 3
//	Course: CPSC 526
//	Author: Zijun Tang
//	Time: Nov 30, 2016
// ----------------------------------------------------------------------------
#include "RPCClient.hpp"

u64 RPCClient::GetNode(const u64& node_id) {
  Node node;
  node.set_node_id(node_id);
  Reply reply;
  ClientContext context;

  Status status = stub_->GetNode(&context, node, &reply);
  if (status.ok()) {
    //cout << reply.message() << endl;
    return reply.message();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    cout << "RPC failed" << endl;
    return 500;
  }
}

u64 RPCClient::AddNode(const u64& node_id) {
    Node node;
    node.set_node_id(node_id);
    Reply reply;
    ClientContext context;

    Status status = stub_->AddNode(&context, node, &reply);
    if (status.ok()) {
      //cout << reply.message() << endl;
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      cout << "RPC failed" << endl;
      return 500;
    }
}
u64 RPCClient::AddEdge(const u64& node_a_id, const u64& node_b_id) {
	Edge edge;
	edge.set_node_a_id(node_a_id);
	edge.set_node_b_id(node_b_id);
	Reply reply;
	ClientContext context;

	Status status = stub_->AddEdge(&context, edge, &reply);
	if (status.ok()) {
	  //cout << reply.message() << endl;
	  return reply.message();
	} else {
	  std::cout << status.error_code() << ": " << status.error_message()
	            << std::endl;
	  cout << "RPC failed" << endl;
	  return 500;
	}
}

u64 RPCClient::RemoveNode(const u64& node_id) {
	Node node;
	node.set_node_id(node_id);
	Reply reply;
	ClientContext context;

	Status status = stub_->RemoveNode(&context, node, &reply);
	if (status.ok()) {
	  //cout << reply.message() << endl;
	  return reply.message();
	} else {
	  std::cout << status.error_code() << ": " << status.error_message()
	            << std::endl;
	  cout << "RPC failed" << endl;
	  return 500;
	}
}

u64 RPCClient::RemoveEdge(const u64& node_a_id, const u64& node_b_id) {
	Edge edge;
	edge.set_node_a_id(node_a_id);
	edge.set_node_b_id(node_b_id);
	Reply reply;
	ClientContext context;

	Status status = stub_->RemoveEdge(&context, edge, &reply);
	if (status.ok()) {
	  //cout << reply.message() << endl;
	  return reply.message();
	} else {
	  std::cout << status.error_code() << ": " << status.error_message()
	            << std::endl;
	  cout << "RPC failed" << endl;
	  return 500;
	}
}
