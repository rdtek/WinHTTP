#pragma once
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include "logging.h"
#include "stringtools.h"
#include "urlinfo.h"
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 1024
using namespace std;

struct app_options {
    URLInfo url_info;
    bool b_verbose_mode;
    bool b_save_response;
    string str_http_method;
    string str_port_num;
    string str_outfile;
};

bool init_connection(SOCKET& conn_socket, URLInfo& url_info);
void send_http_request(SOCKET& conn_socket, app_options& options);
void receive_http_response(SOCKET& conn_socket, app_options& options);
void output_to_file(string& str_filename, char * response_buff);
void get_options(int argc, char* argv[], app_options& options);