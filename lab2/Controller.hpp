//  File: Controller.hpp ------------------------------------------------------------
//  Assignment 2
//  Course: CPSC 526
//  Author: Xi Wang
//  Time: Oct 22, 2016
// ----------------------------------------------------------------------------

#include "Graph.hpp"
#include "SuperBlock.hpp"
#include "Log.hpp"
#include "mongoose.h"
#include "frozen.h"

class Controller{

private:
	static Graph graph;
	static int fd;
	static bool logBlock_full;

	static void setCheck(int fd, u64 offset);

	static bool valid(int fd, u64 offset);

	static bool is_log_full();

	static void writeLog(u32 opcode, u64 node_a_id, u64 node_b_id = 0);

public:
	Controller() = default;

	Controller(int fd){
		this->fd = fd;
		logBlock_full = false;
	}
	
	static void handle_add_node(struct mg_connection *nc, struct http_message *hm);

	static void handle_add_edge(struct mg_connection *nc, struct http_message *hm);

	static void handle_remove_node(struct mg_connection *nc, struct http_message *hm);

	static void handle_remove_edge(struct mg_connection *nc, struct http_message *hm);

	static void handle_get_node(struct mg_connection *nc, struct http_message *hm);

	static void handle_get_edge(struct mg_connection *nc, struct http_message *hm);

	static void handle_get_neighbors(struct mg_connection *nc, struct http_message *hm);

	static void handle_shortest_path(struct mg_connection *nc, struct http_message *hm);

	static void handle_display(struct mg_connection *nc, struct http_message *hm);

	static void handle_checkpoint(struct mg_connection *nc, struct http_message *hm);

	void format();

	void init();
};