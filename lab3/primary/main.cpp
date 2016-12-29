// File: main.cpp -------------------------------------------------------------
//	Assignment 2
//	Course: CPSC 526
//	Author: Xi Wang
//	Time: Oct 22, 2016
// ----------------------------------------------------------------------------
// ***************************************//
// Server 1: No rpc server + 1 rpc client //
// ***************************************//

#include "RPCServer.hpp"

#define NUM_THREADS 2

static char* s_http_port = (char*) "8000";
static string curr_ipaddress = "10.128.0.2";
static string next_ipaddress = "10.128.0.3";
static string curr_rpc_port = "50051";
static string next_rpc_port = "50051";

void *runRPCServer(void *threadarg) {
    string server_address(curr_ipaddress + ":" + curr_rpc_port);
    RPCServer service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    cout << "RPC server listening on " << server_address << endl;
    server->Wait();
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {	
	int flags, opt;
	flags = 0;
	
// Parse command line arguments
	while ((opt = getopt(argc, argv, "b")) != -1) {
		switch (opt) {
			case 'b':
				flags = 1;
				break;
          default: /* '?' */
				fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
				break;
		}		
	}
	if (optind >= argc) {
		printf("Expected argument after options:\n");
		printf("The system uses the default port 8000\n");
		return 1;
	}else if(flags){
		next_ipaddress = string(argv[optind]);
		s_http_port = argv[optind+1];
	}else{
		s_http_port = argv[optind];
	}

    GraphServer graphserver(next_ipaddress, next_rpc_port, s_http_port);
    graphserver.run();
}


