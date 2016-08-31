#pragma once
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <map>
#include "logging.h"
#include "stringtools.h"
#include "urlinfo.h"
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 1024
using namespace std;

struct app_options {
    URLInfo url_info;       //Class holding parts of URL
    bool b_verbose_mode;    //Output extra information
    bool b_save_response;   //Save response to a text file.
    bool b_follow_location; //Follow a redirect location header
    string str_http_method; //HTTP verb e.g. GET, POST, PUT...
    string str_port_num;    //Port number to conntect to on the host
    string str_outfile;     //File path of the output file
};

bool init_connection
    (SOCKET& conn_socket, URLInfo& url_info);

void send_http_request
    (SOCKET& conn_socket, app_options& options);

void receive_http_response
    (SOCKET& conn_socket, app_options& options);

void read_http_headers
    (vector<string> v_response_lines, map<string, string>& mp_headers);

void output_to_file
    (string& str_filename, char * response_buff);

void get_options
    (int argc, char* argv[], app_options& options);