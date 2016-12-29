//  File: GraphServer.cpp ------------------------------------------------------------
//  Assignment 4
//  Course: CPSC 526
//  Author: Xi Wang
//  Time: Dec 19, 2016
// ----------------------------------------------------------------------------
#include "GraphServer.hpp"

Graph GraphServer::graph;

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

	if(node_id % NUM_PARTITION != partnum - 1){
		mg_send_head(nc, 400, 0, NULL);
	}else if(!graph.get_node(node_id)){// Node not exists
		graph.add_node(node_id);
		mg_send_head(nc, 200, body->len, "Content-Type: application/json");
		mg_send(nc, body->p, body->len);
	}else{// Node exists
		//mg_send_response_line(nc, 204, NULL);
		mg_send_head(nc, 204, 0, NULL);
	}
}

// multi-partition
void GraphServer::handle_add_edge(struct mg_connection *nc, struct http_message *hm) {
	u64 node_a_id, node_b_id;

	// Get from body
	struct mg_str *body = &hm->body;
	// Parse json
	json_scanf(body->p, body->len, "{node_a_id:%" SCNu64 ", node_b_id:%" SCNu64 "}", &node_a_id, &node_b_id);

	if(node_a_id == node_b_id){
		mg_send_head(nc, 400, 0, NULL);
		return;
	}

	// assume node_a should in current partition
	if(node_a_id % NUM_PARTITION != partnum - 1){
		swap(node_a_id, node_b_id);
	}
	int currPartnum = node_a_id % NUM_PARTITION + 1;
	int anotherPartnum = node_b_id % NUM_PARTITION + 1;

	if(currPartnum != partnum || anotherPartnum < partnum || !graph.get_node(node_a_id)){
		mg_send_head(nc, 400, 0, NULL);
		return;
	}
	// edge exist
	if(graph.get_edge(node_a_id, node_b_id)){
		mg_send_head(nc, 204, 0, NULL);
		return;
	}

	// b in current partition
	if(anotherPartnum == partnum){
		if(!graph.get_node(node_b_id)){
			//mg_send_response_line(nc, 400, NULL);
			mg_send_head(nc, 400, 0, NULL);
		}else {//Edge not exists
			graph.add_edge(node_a_id, node_b_id);
			graph.add_edge(node_b_id, node_a_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
		}
		return;
	}

	// b not in current partition
	string address = partlist[anotherPartnum-1];
	RPCClient client = RPCClient(grpc::CreateChannel(
      	address, grpc::InsecureChannelCredentials()));

	int code = client.AddEdge(node_b_id, node_a_id);
	switch(code){
		case 200:
			graph.add_edge(node_a_id, node_b_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
			break;
		default:
			mg_send_head(nc, code, 0, NULL);
			break;
	}
}

void GraphServer::handle_remove_node(struct mg_connection *nc, struct http_message *hm) {
	u64 node_id;

	// Get from body
	struct mg_str *body = &hm->body;

	// Parse json
	json_scanf(body->p, body->len, "{node_id:%" SCNu64 "}", &node_id);

	if(graph.get_node(node_id)){// Node exists
		graph.remove_node(node_id);
		mg_send_head(nc, 200, body->len, "Content-Type: application/json");
		mg_send(nc, body->p, body->len);
	}else{// Node not exists
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}
}

// multi-partition
void GraphServer::handle_remove_edge(struct mg_connection *nc, struct http_message *hm) {
	u64 node_a_id, node_b_id;

	// Get from body
	struct mg_str *body = &hm->body;
	// Parse json
	json_scanf(body->p, body->len, "{node_a_id:%" SCNu64 ", node_b_id:%" SCNu64 "}", &node_a_id, &node_b_id);

	// assume node_a should in current partition
	if(node_a_id % NUM_PARTITION != partnum - 1){
		swap(node_a_id, node_b_id);
	}
	int currPartnum = node_a_id % NUM_PARTITION + 1;
	int anotherPartnum = node_b_id % NUM_PARTITION + 1;

	if(currPartnum != partnum || anotherPartnum < partnum || !graph.get_edge(node_a_id, node_b_id)){
		mg_send_head(nc, 400, 0, NULL);
		return;
	}

	//Edge exists and b in current partition
	if(anotherPartnum == partnum){
		graph.remove_edge(node_a_id, node_b_id);
		graph.remove_edge(node_b_id, node_a_id);
		mg_send_head(nc, 200, body->len, "Content-Type: application/json");
		mg_send(nc, body->p, body->len);
		return;
	}
	//Edge exists and b not in current partition
	string address = partlist[anotherPartnum-1];
	RPCClient client = RPCClient(grpc::CreateChannel(
      	address, grpc::InsecureChannelCredentials()));

	int code = client.RemoveEdge(node_b_id, node_a_id);
	switch(code){
		case 200:
			graph.remove_edge(node_a_id, node_b_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
			break;
		default:
			mg_send_head(nc, code, 0, NULL);
			break;
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

// multi-partition
void GraphServer::handle_get_edge(struct mg_connection *nc, struct http_message *hm) {
	u64 node_a_id, node_b_id;

	// Get from body
	struct mg_str *body = &hm->body;
	// Parse json
	json_scanf(body->p, body->len, "{node_a_id:%" SCNu64 ", node_b_id:%" SCNu64 "}", &node_a_id, &node_b_id);

	if(node_a_id == node_b_id){
		mg_send_head(nc, 400, 0, NULL);
		return;
	}

	// assume node_a should in current partition
	if(node_a_id % NUM_PARTITION != partnum - 1){
		swap(node_a_id, node_b_id);
	}
	int currPartnum = node_a_id % NUM_PARTITION + 1;
	int anotherPartnum = node_b_id % NUM_PARTITION + 1;

	if(currPartnum != partnum || anotherPartnum < partnum || !graph.get_node(node_a_id)){
		mg_send_head(nc, 400, 0, NULL);
		return;
	}

	// b not in current partition
	string address = partlist[anotherPartnum-1];
	RPCClient client = RPCClient(grpc::CreateChannel(
      	address, grpc::InsecureChannelCredentials()));

	int code = client.GetNode(node_b_id);
	switch(code){
		case 200:{
			string response;
			if(graph.get_edge(node_a_id, node_b_id))
				response =  "{\"in_graph\":true}";
			else
				response = "{\"in_graph\":false}";
			mg_send_head(nc, 200, response.length(), "Content-Type: application/json");
			mg_printf(nc, "%s", response.c_str());
			break;
		}
		default:
			mg_send_head(nc, code, 0, NULL);
			break;
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
	pthread_mutex_lock (&mtx);
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
			/*else if (mg_vcmp(&func, "/remove_node") == 0) {
				handle_remove_node(nc, hm);
			}*/
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
			/*else if (mg_vcmp(&func, "/shortest_path") == 0) {
				handle_shortest_path(nc, hm);
			}*/
      		else if (mg_vcmp(&func, "/display") == 0) {
				handle_display(nc, hm);
			}
		}
		break;
	default:
		break;
	}
	pthread_mutex_unlock (&mtx);
}
void *GraphServer::run(void *threadarg){
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
