// File: RPCServer.hpp --------------------------------------------------------
//	Assignment 3
//	Course: CPSC 526
//	Author: Xi Wang
//	Time: Nov 29, 2016
// ----------------------------------------------------------------------------
#include "tools.hpp"

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "chain.grpc.pb.h"
#include "GraphServer.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using chain::Node;
using chain::Edge;
using chain::Reply;
using chain::Chain;

class RPCServer final : public Chain::Service{
	Status AddNode(ServerContext* context, const Node* node, Reply* reply) override;
	Status AddEdge(ServerContext* context, const Edge* edge, Reply* reply) override;
	Status RemoveNode(ServerContext* context, const Node* node, Reply* reply) override;
	Status RemoveEdge(ServerContext* context, const Edge* edge, Reply* reply) override;
};