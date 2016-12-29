// File: main.cpp -------------------------------------------------------------
//	Assignment 4
//	Course: CPSC 526
//	Author: Xi Wang
//	Time: Dec 18, 2016
// ----------------------------------------------------------------------------
// ********************************************************//
// Server: 1 rpc server + 1 graph server with rpc client)  //
// ********************************************************//

#include "RPCServer.hpp"

char* s_http_port = (char*) "8000";
int partnum = 0;
vector<string> partlist = {};
pthread_mutex_t mtx;

void *runRPCServer(void *threadarg) {
    string server_address(partlist[partnum-1]);
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
	int pflag, lflag, opt;
	pflag = 0;
    lflag = 0;

    if(argc < 2){
        printf("Expected format <graph_server_port> -p <partnum> -l <partlist>\n");
        return 1;
    }

    char* end;
    long val = strtol(argv[1], &end, 10); //check the port num argument - integer
    if (!end[0] && val >= 0){
        s_http_port = argv[1];
        ++argv;
        --argc;
    }
// Parse command line arguments
	while ((opt = getopt(argc, argv, "p:l")) != -1) {
		switch (opt) {
			case 'p':
				pflag = 1;
                partnum = atoi(optarg);
				break;
            case 'l':
                lflag = 1;
                break;
            default: /* '?' */
				fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
				break;
		}
	}
	if (optind >= argc) {
		printf("Expected argument after options\n");
		return 1;
	}

    if(pflag && lflag){
		while (optind < argc){
            partlist.push_back(string(argv[optind++]));
        }

        if(partlist.size() != NUM_PARTITION){
            printf("Expected %d partitions\n", NUM_PARTITION);
            return 1;
        }
        if(partnum > NUM_PARTITION){
            printf("Expected partnum less than or equal to %d\n", NUM_PARTITION);
            return 1;
        }
	}else{
		printf("Expected options\n");
        return 1;
	}
    pthread_mutex_init(&mtx, NULL);
    pthread_t threads[NUM_THREADS];
    struct thread_data td[NUM_THREADS];
    int rpc_thread;
    int graph_thread;

    for(int i = 0; i < NUM_THREADS; ++i){
        td[i].thread_id = i;
    }

    rpc_thread = pthread_create(&threads[0], NULL, runRPCServer, (void*)&td[0]);
    if (rpc_thread) {
        cout << "Error: unable to create RPC thread, " << rpc_thread << endl;
        exit(-1);
    }

    graph_thread = pthread_create(&threads[1], NULL, GraphServer::run, (void*)&td[1]);
    if (graph_thread) {
        cout << "Error: unable to create Graph thread, " << graph_thread << endl;
        exit(-1);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
         pthread_join(threads[i], NULL);
    }
    pthread_exit(NULL);
}
