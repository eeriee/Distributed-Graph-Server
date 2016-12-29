// File: main.cpp -------------------------------------------------------------
//	Assignment 2
//	Course: CPSC 526
//	Author: Xi Wang
//	Time: Oct 22, 2016
// ----------------------------------------------------------------------------
#include "Controller.hpp"

static char* s_http_port = (char*) "8000";
static char* devfile = (char*) "/dev/sdb";
static struct mg_serve_http_opts s_http_server_opts;
static const struct mg_str s_post_method = MG_MK_STR("POST");

static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
	return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}

static int is_equal(const struct mg_str *s1, const struct mg_str *s2) {
	return s1->len == s2->len && memcmp(s1->p, s2->p, s2->len) == 0;
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	static const struct mg_str api_prefix = MG_MK_STR("/api/v1");
	struct http_message *hm = (struct http_message *) ev_data;
	struct mg_str func;

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		if (has_prefix(&hm->uri, &api_prefix) && is_equal(&hm->method, &s_post_method)) {
			func.p = hm->uri.p + api_prefix.len;
			func.len = hm->uri.len - api_prefix.len;
			if (mg_vcmp(&func, "/add_node") == 0) {
				Controller::handle_add_node(nc, hm);
			}
			else if (mg_vcmp(&func, "/add_edge") == 0) {
				Controller::handle_add_edge(nc, hm);
			}
			else if (mg_vcmp(&func, "/remove_node") == 0) {
				Controller::handle_remove_node(nc, hm);
			}
			else if (mg_vcmp(&func, "/remove_edge") == 0) {
				Controller::handle_remove_edge(nc, hm);
			}
			else if (mg_vcmp(&func, "/get_node") == 0) {
				Controller::handle_get_node(nc, hm);
			}
			else if (mg_vcmp(&func, "/get_edge") == 0) {
				Controller::handle_get_edge(nc, hm);
			}
			else if (mg_vcmp(&func, "/get_neighbors") == 0) {
				Controller::handle_get_neighbors(nc, hm);
			}
			else if (mg_vcmp(&func, "/shortest_path") == 0) {
				Controller::handle_shortest_path(nc, hm);
			}
      		else if (mg_vcmp(&func, "/checkpoint") == 0) {
        		Controller::handle_checkpoint(nc, hm);
      		}
      		else if (mg_vcmp(&func, "/display") == 0) {
				Controller::handle_display(nc, hm);
			}
		}
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[]) {
	struct mg_mgr mgr;
	struct mg_connection *nc;

	int flags, opt, fd;
	flags = 0;
	fd = -1;
	
// Parse command line arguments
	while ((opt = getopt(argc, argv, "f")) != -1) {
		switch (opt) {
			case 'f':
				flags = 1;
				break;
          default: /* '?' */
				fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
				break;
		}
		
	}
	if (optind >= argc) {
		printf("Expected argument after options:\n");
		printf("The system uses the default port 8000 and devfile /dev/sdb\n");
		//return 1;
	}else{
		s_http_port = argv[optind];
		devfile = argv[optind+1];
	}

	if ((fd = open(devfile, O_RDWR, 0)) == -1){
		printf("Cannot open devfile\n");
		return 1;
	}
  	
  	Controller controller(fd);
  	if(flags == 1){
    	controller.format();
  	}else{
  		controller.init();
  	}

	mg_mgr_init(&mgr, NULL);

	nc = mg_bind(&mgr, s_http_port, ev_handler);
	if (nc == NULL) {
		printf("Failed to create listener\n");
		return 1;
	}	

	// Set up HTTP server parameters
	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = ".";  // Serve current directory
	s_http_server_opts.enable_directory_listing = "yes";

	printf("Starting graph server on port %s\n", s_http_port);

	for (;;) {
		 	mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return 0;
}
