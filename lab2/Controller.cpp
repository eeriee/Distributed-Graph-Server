//  File: Controller.cpp ------------------------------------------------------------
//  Assignment 2
//  Course: CPSC 526
//  Author: Xi Wang
//  Time: Oct 22, 2016
// ----------------------------------------------------------------------------

#include "Controller.hpp"

Graph Controller:: graph;
int Controller:: fd;
bool Controller::logBlock_full;

bool Controller::is_log_full(){
	SuperBlock* super_block = (SuperBlock*) mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, fd, 0);
	u32 size_log = super_block->size_log;
	munmap(super_block, BLOCK_SIZE);
	return size_log == TOTAL_LOG && logBlock_full;
}

void Controller::writeLog(u32 opcode, u64 node_a_id, u64 node_b_id){
	SuperBlock* super_block = (SuperBlock*) mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	u32 current_block = super_block->size_log;
	u32 generation_number = super_block->current_generation;
	u64 offset = current_block*BLOCK_SIZE;
	if(logBlock_full || current_block== 0){
		offset += BLOCK_SIZE;
		++(super_block->size_log);
	}
	munmap(super_block, BLOCK_SIZE);
    setCheck(fd, 0);
	//cout << "current_block: " << current_block <<endl <<endl;
    LogBlock* log = (LogBlock*)mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    u32 current_entry;
    if(!logBlock_full && current_block != 0){
        current_entry = log->entry_count;
        ++(log->entry_count);
        if(current_entry >= TOTAL_ENTRY-1){
        	logBlock_full = true;
        }
    }else{
        current_entry = 0;
        log->entry_count = 1;
        log->generation_number = generation_number;
        if(logBlock_full) logBlock_full = false;
    }
    //cout << "current_entry: " << current_entry << endl;
    log->entries[current_entry].opcode = opcode;
    log->entries[current_entry].node_a_id = node_a_id;
    log->entries[current_entry].node_b_id = node_b_id;
    /*switch(current_entry){
    	case 130:
    	case 131:
    	case 136:
    		cout << "opcode: " << log->entries[current_entry].opcode << endl;
    		cout << "a id: " << log->entries[current_entry].node_a_id<< endl;
    		cout << "b id: " << log->entries[current_entry].node_b_id << endl;
    		cout << "input op: " << opcode << endl;
    		cout << "input aid " << node_a_id << endl;
    		cout << "input bid " << node_b_id << endl;
    		break;
    	default:
    		break;
    }*/
    munmap(log, BLOCK_SIZE);
    setCheck(fd, offset);
}
void Controller::handle_add_node(struct mg_connection *nc, struct http_message *hm) {
	u64 node_id;

// Get from body
	struct mg_str *body = &hm->body;

// Parse json
	json_scanf(body->p, body->len, "{node_id:%" SCNu64 "}", &node_id);

	if(!graph.get_node(node_id)){// Node not exists
		if(is_log_full()){
			mg_send_head(nc, 507, 0, NULL);
		}else{	
			writeLog(0, node_id);
			graph.add_node(node_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
		}
	}else{// Node exists
		//mg_send_response_line(nc, 204, NULL);
		mg_send_head(nc, 204, 0, NULL);
	}
}

void Controller::handle_add_edge(struct mg_connection *nc, struct http_message *hm) {
	u64 node_a_id, node_b_id;

// Get from body
	struct mg_str *body = &hm->body;
// Parse json
	json_scanf(body->p, body->len, "{node_a_id:%" SCNu64 ", node_b_id:%" SCNu64 "}", &node_a_id, &node_b_id);


	if(!graph.get_node(node_a_id) || !graph.get_node(node_b_id) || node_a_id == node_b_id){ // One or both of the specified nodes are not in the graph
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}else if(!graph.get_edge(node_a_id, node_b_id)){//Edge not exists
		if(is_log_full()){
			mg_send_head(nc, 507, 0, NULL);
		}else{	
			writeLog(1, node_a_id, node_b_id);
			graph.add_edge(node_a_id, node_b_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
		}
	}else{//Edge exists
		//mg_send_response_line(nc, 204, NULL);
		mg_send_head(nc, 204, 0, NULL);
	}
}

void Controller::handle_remove_node(struct mg_connection *nc, struct http_message *hm) {
	u64 node_id;

	// Get from body
	struct mg_str *body = &hm->body;

	// Parse json
	json_scanf(body->p, body->len, "{node_id:%" SCNu64 "}", &node_id);

	if(graph.get_node(node_id)){// Node exists
		if(is_log_full()){
			mg_send_head(nc, 507, 0, NULL);
		}else{	
			writeLog(2, node_id);
			graph.remove_node(node_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
		}
	}else{// Node not exists
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}
}

void Controller::handle_remove_edge(struct mg_connection *nc, struct http_message *hm) {
	u64 node_a_id, node_b_id;

	// Get from body
	struct mg_str *body = &hm->body;
	// Parse json
	json_scanf(body->p, body->len, "{node_a_id:%" SCNu64 ", node_b_id:%" SCNu64 "}", &node_a_id, &node_b_id);

	if(graph.get_edge(node_a_id, node_b_id)){//Edge exists
		if(is_log_full()){
			mg_send_head(nc, 507, 0, NULL);
		}else{	
			writeLog(3, node_a_id, node_b_id);
			graph.remove_edge(node_a_id, node_b_id);
			mg_send_head(nc, 200, body->len, "Content-Type: application/json");
			mg_send(nc, body->p, body->len);
		}
	}else{
		//mg_send_response_line(nc, 400, NULL);
		mg_send_head(nc, 400, 0, NULL);
	}
}

void Controller::handle_get_node(struct mg_connection *nc, struct http_message *hm) {
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

void Controller::handle_get_edge(struct mg_connection *nc, struct http_message *hm) {
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

void Controller::handle_get_neighbors(struct mg_connection *nc, struct http_message *hm) {
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

void Controller::handle_shortest_path(struct mg_connection *nc, struct http_message *hm) {
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

void Controller::handle_display(struct mg_connection *nc, struct http_message *hm) {
	graph.display();
}

void Controller::handle_checkpoint(struct mg_connection *nc, struct http_message *hm) {
	string res = graph.serialize();
	u64 len = res.size();
	if(len > CHECKPOINT_SIZE){
		mg_send_head(nc, 507, 0, NULL);
	}else{
		u32 block_count = len / BLOCK_SIZE;
		if(len%BLOCK_SIZE && len > 1) ++block_count;
		//cout << len << endl;
		u32 pos = 0;
		//for(u32 i = 0; i < block_count; ++i){
		char* s = (char*) mmap(NULL, block_count * BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, LOG_SIZE);
		//string* s = (string*)mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, LOG_SIZE + i*BLOCK_SIZE);
		strcpy(s, res.c_str()); 
		munmap(s, BLOCK_SIZE);
		pos += BLOCK_SIZE;
		//}
	  	
	  	SuperBlock* super_block = (SuperBlock*) mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	  	++(super_block->current_generation);
	  	super_block->block_count = block_count;
	  	super_block->size_log = 0;
	  	super_block->start_log = 1;
	  	munmap(super_block, BLOCK_SIZE);
    	setCheck(fd, 0);
	  	mg_send_head(nc, 200, 0, NULL);
  	}  
}

void Controller::setCheck(int fd, u64 offset) {
    u64* block = (u64*) mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
    u64 res = 0;
    for (int i = 1; i < 512; i++) {
        res = res ^ block[i];
    }
    u64 checkSum = res + 1;
    block[0] = checkSum;
    munmap(block, BLOCK_SIZE);
}

bool Controller::valid(int fd, u64 offset) {
    u64* block = (u64*) mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, fd, offset);
    u64 res = block[0] - 1;
    for (int i = 1; i < 512; i++) {
        res = res ^ block[i];
    }
    //cout << "valid: " <<res<<endl;
    munmap(block, BLOCK_SIZE);
    if (res == 0) {
        return true;
    } else {
        return false;
    }
}

void Controller::format() {
	SuperBlock* super_block = (SuperBlock*) mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (valid(fd, 0)) {
        ++(super_block->current_generation);
    } else {
        super_block->current_generation = 0;      
    }
    super_block->start_log = 1;
    super_block->size_log = 0;
  	super_block->block_count = 0;

    logBlock_full = false;
    munmap(super_block, BLOCK_SIZE);
    setCheck(fd, 0);
}

void Controller::init(){
	if(valid(fd, 0)) {
		SuperBlock* super_block = (SuperBlock*) mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, fd, 0);
		u32 block_count = super_block->block_count;
		u32 generation_number = super_block->current_generation;
		u32 size_log = super_block->size_log;
		munmap(super_block, BLOCK_SIZE);

		if(block_count != 0){
			
			char* s = (char*)mmap(NULL, block_count*BLOCK_SIZE, PROT_READ, MAP_SHARED, fd, LOG_SIZE);
			string s_graph(s);
			munmap(s, BLOCK_SIZE);
			
			graph.deserialize(s_graph);
		}
		
		for (u32 i = 1; i <= size_log; ++i) {
			u64 offset = i*BLOCK_SIZE;
            LogBlock* l = (LogBlock*)mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, fd, offset);
            if (l->generation_number != generation_number){
            	printf("%" PRIu32 " block has different generation number.\n", i);
            	break;
            } 
            if (!valid(fd, i*BLOCK_SIZE)){
            	printf("%" PRIu32 " block has invalid checksum.\n", i);
            	break;
            } 

            for (u32 j = 0; j < l->entry_count; ++j) {
                switch (l->entries[j].opcode) {
                    case 0:
                        graph.add_node(l->entries[j].node_a_id);
                        break;
                    case 1:
                        graph.add_edge(l->entries[j].node_a_id, l->entries[j].node_b_id);
                        break;
                    case 2:
                        graph.remove_node(l->entries[j].node_a_id);
                        break;
                    case 3:
                        graph.remove_edge(l->entries[j].node_a_id, l->entries[j].node_b_id);
                        break;
                    default:
                    	cout << "Error in controller:init:switch" << endl;
                    	cout << "block: " << i << " entry: " << j << endl;
                    	cout << "opcode: " << l->entries[j].opcode << endl;
                    	cout << "node_a_id: " << l->entries[j].node_a_id << endl;
                    	cout << "node_b_id: " << l->entries[j].node_b_id << endl;                        
                        break;
                }
            }
            if(l->entry_count == TOTAL_ENTRY) logBlock_full = true;
            munmap(l, BLOCK_SIZE);
        }
      
	}else{
		printf("Normal startup with invalid checksum\n");
		exit(1);
	}
}
