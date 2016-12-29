// File: main.cpp -------------------------------------------------------------
//	Assignment 2
//	Course: CPSC 526
//	Author: Xi Wang
//	Time: Oct 22, 2016
// ----------------------------------------------------------------------------
// ***************************************//
// Server 2: 1 rpc server + 1 rpc client  //
// ***************************************//

#include "RPCServer.hpp"

#define NUM_THREADS 2

static char* s_http_port = (char*) "8000";
static string curr_ipaddress = "10.128.0.3";
static string next_ipaddress = "10.128.0.4";
static string curr_rpc_port = "50051";
static string next_rpc_port = "50051";

void *runRPCServer(void *threadarg) {
    string server_address(curr_ipaddress + ":" + curr_rpc_port);
    RPCServer service(next_ipaddress, next_rpc_port);

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

    pthread_t threads[NUM_THREADS];
    struct thread_data td[NUM_THREADS];


    int rpc_thread;
    int graph_thread;

    td[0].thread_id = 0;
    rpc_thread = pthread_create(&threads[0], NULL, runRPCServer, (void*)&td[0]);
    if (rpc_thread) {
        cout << "Error: unable to create RPC thread, " << rpc_thread << endl;
        exit(-1);
    }

    td[1].thread_id = 1;
    td[1].message = s_http_port;
    graph_thread = pthread_create(&threads[1], NULL, GraphServer::run, (void*)&td[1]);
    if (graph_thread) {
        cout << "Error: unable to create Graph thread, " << graph_thread << endl;
        exit(-1);
    }

    pthread_exit(NULL);
}


