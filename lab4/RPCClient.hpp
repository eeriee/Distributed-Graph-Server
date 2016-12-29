// File: RPCClient.hpp --------------------------------------------------------
//	Assignment 3
//	Course: CPSC 526
//	Author: Zijun Tang
//	Time: Nov 30, 2016
// ----------------------------------------------------------------------------
#include "tools.hpp"

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

#include "chain.grpc.pb.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using chain::Chain;
using chain::Edge;
using chain::Node;
using chain::Reply;

class RPCClient{
private:
	std::unique_ptr<Chain::Stub> stub_;
public:
	RPCClient(std::shared_ptr<Channel> channel)
    : stub_(Chain::NewStub(channel)) {}


	u64 GetNode(const u64& node_id);
	u64 AddNode(const u64& node_id);
	u64 AddEdge(const u64& node_a_id, const u64& node_b_id);
	u64 RemoveNode(const u64& node_id);
	u64 RemoveEdge(const u64& node_a_id, const u64& node_b_id);
};
