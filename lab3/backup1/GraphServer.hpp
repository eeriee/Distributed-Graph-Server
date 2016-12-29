//  File: GraphServer.hpp ------------------------------------------------------------
//  Assignment 2
//  Course: CPSC 526
//  Author: Xi Wang
//  Time: Oct 22, 2016
// ----------------------------------------------------------------------------

#include "Graph.hpp"

class RPCServer;

class GraphServer{
	friend class RPCServer;
private:
	static Graph graph;
	static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix);

	static int is_equal(const struct mg_str *s1, const struct mg_str *s2);

	static void handle_add_node(struct mg_connection *nc, struct http_message *hm);

	static void handle_add_edge(struct mg_connection *nc, struct http_message *hm);

	static void handle_remove_node(struct mg_connection *nc, struct http_message *hm);

	static void handle_remove_edge(struct mg_connection *nc, struct http_message *hm);

	static void handle_get_node(struct mg_connection *nc, struct http_message *hm);

	static void handle_get_edge(struct mg_connection *nc, struct http_message *hm);

	static void handle_get_neighbors(struct mg_connection *nc, struct http_message *hm);

	static void handle_shortest_path(struct mg_connection *nc, struct http_message *hm);

	static void handle_display(struct mg_connection *nc, struct http_message *hm);

	static void ev_handler(struct mg_connection *nc, int ev, void *ev_data);
public:
	static void* run(void* threadarg);
};
