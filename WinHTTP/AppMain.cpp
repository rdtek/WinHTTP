#include "AppMain.h"

//Usefule MSDN docs for Winsock Client Application
//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530750(v=vs.85).aspx

//==========================================================================
// main - application entry point
//==========================================================================
int main(int argc, char* argv[]) {

    SOCKET conn_socket = INVALID_SOCKET;
    struct app_options options;
    
    //Set default options
    options.b_verbose_mode = false;
    options.b_save_response = false;
    options.str_http_method = "GET";
    options.str_port_num = DEFAULT_PORT;

    get_options(argc, argv, options);

    //Setup connection, send and receive
    if (init_connection(conn_socket, options.url_info)) {
        send_http_request(conn_socket, options);
        receive_http_response(conn_socket, options);
    };

#ifdef _DEBUG
    std::cout << "\n\nPress a key to quit.";
    std::cin.get();
#endif

    return 0;
}

//==========================================================================
// get_options - get options from command line args
//==========================================================================
void get_options(int argc, char* argv[], app_options& options) {

    if (argc <= 1) {
        cout << "Not enough or invalid arguments.\n";
        cout << "URL argument required.";
        Sleep(2000);
        exit(0);
    }

    string str_url(argv[1]);
    StringTools::parseURL(str_url, options.url_info);

    for (int i = 1; i < argc; i++) {

        string strArg(argv[i]);

        if (strArg == "--output" || strArg == "-o") {
            //Save response to file
            options.b_save_response = true;
            options.str_outfile = string(argv[i + 1]);
        }
        else if (strArg == "--method" || strArg == "-m") {
            options.str_http_method = string(argv[i + 1]);
            cout << "\n** METHOD " << options.str_http_method << "**\n";
        }
        else if (strArg == "--verbose" || strArg == "-v") {
            options.b_verbose_mode = true;
        }
        else if (strArg == "--location" || strArg == "-L") {
            options.b_follow_location = true;
        }
    }
}

//==========================================================================
// init_connection - Initialize Winsock WS2_32.dll and attempt to connect
//==========================================================================
bool init_connection(SOCKET& conn_socket, URLInfo& url_info) {
    
    int iResult;
    WSADATA wsaData;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo *result = NULL,
        *ptr = NULL,
        hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    // iResult = getaddrinfo(strURL.c_str(), DEFAULT_PORT, &hints, &result);
    iResult = getaddrinfo(url_info.host.c_str(), DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("\ngetaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    //==========================================================================
    // CONNECT - Attempt to connect to the first address returned by getaddrinfo
    //==========================================================================
    ptr = result;
    conn_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (conn_socket == INVALID_SOCKET) {
        printf("\nError at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server.
    iResult = connect(conn_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("\nSocket error.\n");
        closesocket(conn_socket);
        conn_socket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (conn_socket == INVALID_SOCKET) {
        printf("\nUnable to connect to server!\n");
        WSACleanup();
        return 0;
    }
}

//==========================================================================
// send_http_request - Send an initial buffer
//==========================================================================
void send_http_request(SOCKET& conn_socket, app_options& options) {
    
    int iResult;
    std::string strReq;

    strReq.append(options.str_http_method);
    strReq.append(" ");
    strReq.append(options.url_info.path);
    strReq.append(" HTTP/1.0");
    strReq.append("\r\nHOST: ");
    strReq.append(options.url_info.host);
    strReq.append(":");
    strReq.append(options.str_port_num);
    strReq.append("\r\n\r\n");

    iResult = send(conn_socket, strReq.c_str(), (int)strlen(strReq.c_str()), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(conn_socket);
        WSACleanup();
        return;
    }

    if (options.b_verbose_mode && iResult > 0) {
        printf("\nBytes Sent: %ld", iResult);
        printf("\n%s", strReq.c_str());
    }

    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    iResult = shutdown(conn_socket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(conn_socket);
        WSACleanup();
        return;
    }
}

//==========================================================================
// receive_http_response - read data until the server closes the connection
//==========================================================================
void receive_http_response(SOCKET& conn_socket, app_options& options) {

    int iResult;
    int rcv_buff_length = DEFAULT_BUFLEN;
    int loop_count = 0;
    map<string, string> mp_http_headers;

    //Loop to receive the raw reponse from the socket
    do {
        char receive_buff[DEFAULT_BUFLEN];
        iResult = recv(conn_socket, receive_buff, rcv_buff_length, 0);

        if (options.b_save_response) 
            output_to_file(options.str_outfile, receive_buff);

        if (iResult > 0) {
            string str_resp_buff = string(receive_buff);
            
            if (options.b_verbose_mode) 
                printf("\rBytes received: %d\n", iResult);
            
            if (loop_count == 0) {

                //Declare variables
                string str_response(receive_buff);
                vector<string> v_split_items;
                vector<string> v_response_lines;
                string str_status;

                //Read the status code
                v_split_items = StringTools::split(str_response, ' ');
                str_status = v_split_items[1];

                //Read the headers
                v_response_lines = StringTools::split(str_response, '\n');
                read_http_headers(v_response_lines, mp_http_headers);

#ifdef _DEBUG
                //Print headers
                for (auto kvp : mp_http_headers)
                    cout << "\nHEADER ITEM [" << kvp.first << "] [" << kvp.second << "]";
#endif // _DEBUG
            }
            cout << str_resp_buff;
        }
        else if (iResult == 0) {
            if (options.b_verbose_mode) printf("\nConnection closed\n");
        }
        else {
            printf("recv failed: %d\n", WSAGetLastError());
        }
        loop_count++;
    } while (iResult > 0);

    closesocket(conn_socket);
    WSACleanup();

    //Follow redirect if location header present
    if (options.b_follow_location) {
        map<string, string>::iterator it = mp_http_headers.find("Location");
        if (it != mp_http_headers.end()) {
            string str_location_url = it->second;
            //cout << "LOCATION " << str_location_url;

            StringTools::parseURL(str_location_url, options.url_info);
            if (init_connection(conn_socket, options.url_info)) {
                send_http_request(conn_socket, options);
                receive_http_response(conn_socket, options);
            };
        }
    }
}

//==========================================================================
// read_http_headers - read the headers into a map structure
//==========================================================================
void read_http_headers(vector<string> v_response_lines, map<string, string>& mp_headers) {
    
    for (vector<string>::const_iterator it = v_response_lines.begin(); 
        it != v_response_lines.end(); ++it) {

        string str_line = *it;
        str_line = StringTools::trim(str_line);

        bool found_html = str_line.find("<HTML") == 0 || str_line.find("<html") == 0;
        if (found_html) {
            //cout << "\nFOUND HTML";
            break;
        }

        //Split the line to get the header key value pair
        int split_pos = str_line.find(':', 0);
        if (split_pos >= 1) {
            string str_header_key = StringTools::trim(str_line.substr(0, split_pos));
            string str_header_val = StringTools::trim(str_line.substr(split_pos + 1, string::npos));
            mp_headers[str_header_key] = str_header_val;
        }
        else {
            //cout << str_line;
        }
    }
}

//==========================================================================
// output_to_file - save response to file
//==========================================================================
void output_to_file(string& str_filename, char * response_buff) {
    std::ofstream outfile;
    if (str_filename.empty()) 
        str_filename = "response.html";
    outfile.open(str_filename, std::ios::app);
    outfile << response_buff;
}