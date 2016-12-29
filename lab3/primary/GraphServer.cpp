//  File: GraphServer.cpp ------------------------------------------------------------
//  Assignment 2
//  Course: CPSC 526
//  Author: Xi Wang
//  Time: Oct 22, 2016
// ----------------------------------------------------------------------------
#include "GraphServer.hpp"

Graph GraphServer::graph;
string GraphServer::address = "104.197.141.213:2000";

int GraphServer::has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
	return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}

int GraphServer::is_equal(const struct mg_str *s1, const struct mg_str *s2) {
	return s1->len == s2->len && memcmp(s1->p, s2->p, s2->len) == 0;
}

void GraphServer::handle_add_node(struct mg_connection *nc, struct http_message *hm) {
	u64 node_id;

	// Get from body
	struct mg_str *body = &hm->body;

	// Parse json
	json_scanf(body->p, body->len, "{node_id:%" SCNu64 "}", &node_id);

	if(!graph.get_node(node_id)){// Node not exists
		RPCClient client = RPCClient(grpc::CreateChannel(
      	address, grpc::InsecureChannelCredentials()));
		bool ok = client.AddNode(node_id);
		if(ok){
			graph.add_node(node_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
		}else{
			mg_send_head(nc, 500, 0, NULL);
		}
	}else{// Node exists
		//mg_send_response_line(nc, 204, NULL);
		mg_send_head(nc, 204, 0, NULL);
	}
}

void GraphServer::handle_add_edge(struct mg_connection *nc, struct http_message *hm) {
	u64 node_a_id, node_b_id;

	// Get from body
	struct mg_str *body = &hm->body;
	// Parse json
	json_scanf(body->p, body->len, "{node_a_id:%" SCNu64 ", node_b_id:%" SCNu64 "}", &node_a_id, &node_b_id);


	if(!graph.get_node(node_a_id) || !graph.get_node(node_b_id) || node_a_id == node_b_id){ // One or both of the specified nodes are not in the graph
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}else if(!graph.get_edge(node_a_id, node_b_id)){//Edge not exists
		RPCClient client = RPCClient(grpc::CreateChannel(
      	address, grpc::InsecureChannelCredentials()));
		bool ok = client.AddEdge(node_a_id, node_b_id);
		if(ok){
			graph.add_edge(node_a_id, node_b_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
		}else{
			mg_send_head(nc, 500, 0, NULL);
		}
	}else{//Edge exists
		//mg_send_response_line(nc, 204, NULL);
		mg_send_head(nc, 204, 0, NULL);
	}
}

void GraphServer::handle_remove_node(struct mg_connection *nc, struct http_message *hm) {
	u64 node_id;

	// Get from body
	struct mg_str *body = &hm->body;

	// Parse json
	json_scanf(body->p, body->len, "{node_id:%" SCNu64 "}", &node_id);

	if(graph.get_node(node_id)){// Node exists
		RPCClient client = RPCClient(grpc::CreateChannel(
      	address, grpc::InsecureChannelCredentials()));
		bool ok = client.RemoveNode(node_id);
		if(ok){
			graph.remove_node(node_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
		}else{
			mg_send_head(nc, 500, 0, NULL);
		}
	}else{// Node not exists
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}
}

void GraphServer::handle_remove_edge(struct mg_connection *nc, struct http_message *hm) {
	u64 node_a_id, node_b_id;

	// Get from body
	struct mg_str *body = &hm->body;
	// Parse json
	json_scanf(body->p, body->len, "{node_a_id:%" SCNu64 ", node_b_id:%" SCNu64 "}", &node_a_id, &node_b_id);

	if(graph.get_edge(node_a_id, node_b_id)){//Edge exists
		RPCClient client = RPCClient(grpc::CreateChannel(
      	address, grpc::InsecureChannelCredentials()));
		bool ok = client.RemoveEdge(node_a_id, node_b_id);
		if(ok){
			graph.remove_edge(node_a_id, node_b_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
		}else{
			mg_send_head(nc, 500, 0, NULL);
		}
	}else{
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}
}

void GraphServer::handle_get_node(struct mg_connection *nc, struct http_message *hm) {
	u64 node_id;

	// Get from body
	struct mg_str *body = &hm->body;

	// Parse json
	json_scanf(body->p, body->len, "{node_id:%" SCNu64 "}", &node_id);

	string response;
	if(graph.get_node(node_id))
		response =  "{\"in_graph\":true}";
	else
		response = "{\"in_graph\":false}";
	mg_send_head(nc, 200, response.length(), "Content-Type: application/json");
	mg_printf(nc, "%s", response.c_str());
}

void GraphServer::handle_get_edge(struct mg_connection *nc, struct http_message *hm) {
	u64 node_a_id, node_b_id;

	// Get from body
	struct mg_str *body = &hm->body;
	// Parse json
	json_scanf(body->p, body->len, "{node_a_id:%" SCNu64 ", node_b_id:%" SCNu64 "}", &node_a_id, &node_b_id);

	if(!graph.get_node(node_a_id) || !graph.get_node(node_b_id) || node_a_id == node_b_id){ // One or both of the specified nodes are not in the graph
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}else{
		string response;
		if(graph.get_edge(node_a_id, node_b_id))
			response = "{\"in_graph\":true}";
		else
			response = "{\"in_graph\":false}";
		mg_send_head(nc, 200, response.length(), "Content-Type: application/json");
		mg_printf(nc, "%s", response.c_str());
	}
}

void GraphServer::handle_get_neighbors(struct mg_connection *nc, struct http_message *hm) {
	u64 node_id;

	// Get from body
	struct mg_str *body = &hm->body;

	// Parse json
	json_scanf(body->p, body->len, "{node_id:%" SCNu64 "}", &node_id);


	if(!graph.get_node(node_id)){ // Node not in the graph
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}else{
		set<u64> neighbors = graph.get_neighbors(node_id);
		string response = "{\"node_id\":" + to_string(node_id) + ",\"neighbors\":[";
		bool first = true;
		for(u64 ne: neighbors){
			if(first){
				first = false;
				response += to_string(ne);
			}else{
				response += "," + to_string(ne);
			}
		}
		response += "]}";
		mg_send_head(nc, 200, response.length(), "Content-Type: application/json");
		mg_printf(nc, "%s", response.c_str());
	}
}

void GraphServer::handle_shortest_path(struct mg_connection *nc, struct http_message *hm) {
	u64 node_a_id, node_b_id;

	// Get from body
	struct mg_str *body = &hm->body;
	// Parse json
	json_scanf(body->p, body->len, "{node_a_id:%" SCNu64 ", node_b_id:%" SCNu64 "}", &node_a_id, &node_b_id);

	if(!graph.get_node(node_a_id) || !graph.get_node(node_b_id)){ // One or both of the specified nodes are not in the graph
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}else{
		u64 min_dist = graph.shortest_path(node_a_id, node_b_id);
		if(min_dist == 0){//No path
			//mg_send_response_line(nc, 204, NULL);
			mg_send_head(nc, 204, 0, NULL);
		}else{
			string response = "{\"distance\":" + to_string(min_dist) + "}";
			mg_send_head(nc, 200, response.length(), "Content-Type: application/json");
			mg_printf(nc, "%s", response.c_str());
		}
	}
}

void GraphServer::handle_display(struct mg_connection *nc, struct http_message *hm) {
	graph.display();
}

void GraphServer::ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	static const struct mg_str api_prefix = MG_MK_STR("/api/v1");
	static const struct mg_str s_post_method = MG_MK_STR("POST");
	struct http_message *hm = (struct http_message *) ev_data;
	struct mg_str func;

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		if (has_prefix(&hm->uri, &api_prefix) && is_equal(&hm->method, &s_post_method)) {
			func.p = hm->uri.p + api_prefix.len;
			func.len = hm->uri.len - api_prefix.len;
			if (mg_vcmp(&func, "/add_node") == 0) {
				handle_add_node(nc, hm);
			}
			else if (mg_vcmp(&func, "/add_edge") == 0) {
				handle_add_edge(nc, hm);
			}
			else if (mg_vcmp(&func, "/remove_node") == 0) {
				handle_remove_node(nc, hm);
			}
			else if (mg_vcmp(&func, "/remove_edge") == 0) {
				handle_remove_edge(nc, hm);
			}
			else if (mg_vcmp(&func, "/get_node") == 0) {
				handle_get_node(nc, hm);
			}
			else if (mg_vcmp(&func, "/get_edge") == 0) {
				handle_get_edge(nc, hm);
			}
			else if (mg_vcmp(&func, "/get_neighbors") == 0) {
				handle_get_neighbors(nc, hm);
			}
			else if (mg_vcmp(&func, "/shortest_path") == 0) {
				handle_shortest_path(nc, hm);
			}
      		else if (mg_vcmp(&func, "/display") == 0) {
				handle_display(nc, hm);
			}
		}
		break;
	default:
		break;
	}
}

void GraphServer::run(){
	struct mg_mgr mgr;
	struct mg_connection *nc;
	struct mg_serve_http_opts s_http_server_opts;

	mg_mgr_init(&mgr, NULL);

	nc = mg_bind(&mgr, s_http_port, ev_handler);
	if (nc == NULL) {
		printf("Failed to create listener\n");
		exit(1);
	}	

	// Set up HTTP server parameters
	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = ".";  // Serve current directory
	s_http_server_opts.enable_directory_listing = "yes";

	printf("Starting graph server on s_http_port %s\n", s_http_port);

	for (;;) {
		 mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
}
