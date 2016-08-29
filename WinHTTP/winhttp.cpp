#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include "logging.h"
#include "stringtools.h"

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 1024
using namespace std;

//MSDN Winsock Client Application
//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530750(v=vs.85).aspx

int main(int argc, char* argv[]) {

    WSADATA wsaData;
    int iResult;
    bool bSaveResponse = false;
    string strOutFilePath;
    string strHttpMethod = "GET";

    //==========================================================================
    // CHECK ARGS
    //==========================================================================
    if(argc <= 1) {
        cout << "Not enough or invalid arguments.\n";
        cout << "URL argument required.";
        Sleep(2000);
        exit(0);
    }

    string strURL(argv[1]);
    StringTools::parseURL(strURL);

    for (int i = 1; i < argc; i++) { 
        
        cout << "\n[" << argv[i] << "]";
        string strArg(argv[i]);

        if (strArg == "--output") {
            //Save response to file
            cout << "\nSAVE TRUE\n";
            bSaveResponse = true;
            strOutFilePath = string(argv[i + 1]);
        }
        else if (argv[i] == "--method") {
            strHttpMethod = string(argv[i + 1]);
            cout << "\n**METHOD " << strHttpMethod << "**\n";
        }
        
        std::cout << argv[i] << " ";
    }

    //==========================================================================
    // INIT - Initialize Winsock, initiate use of WS2_32.dll.
    //==========================================================================
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
    iResult = getaddrinfo(strURL.c_str(), DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("\ngetaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;

    //==========================================================================
    // CONNECT - Attempt to connect to the first address returned by getaddrinfo
    //==========================================================================
    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("\nError at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server.
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("\nSocket error.\n");
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("\nUnable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    //==========================================================================
    // SEND REQUEST - Send an initial buffer
    //==========================================================================
    int recvbuflen = DEFAULT_BUFLEN;

    std::string strHttpReq;
    strHttpReq.append(strHttpMethod);
    strHttpReq.append(" /services/?fg=1 HTTP/1.0");
    strHttpReq.append("\r\nHOST: ");
    strHttpReq.append("www.google.com.tw:80");
    strHttpReq.append("\r\n\r\n");

    char recvbuf[DEFAULT_BUFLEN];

    iResult = send(ConnectSocket, strHttpReq.c_str(), (int)strlen(strHttpReq.c_str()), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("\nBytes Sent: %ld\n", iResult);
    if (iResult > 0) {
        printf("\n=== Sent Message: ===\n%s\n", strHttpReq.c_str());
    }

    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    //==========================================================================
    // RECEIVE RESPONSE - read data until the server closes the connection
    //==========================================================================
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        
        std::ofstream outfile;
        
        if (bSaveResponse) {
            if (strOutFilePath.empty()) strOutFilePath = "response.html";
            outfile.open(strOutFilePath, std::ios::app);
            outfile << recvbuf;
        }
        
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            //printf("\n=== Received Message ===\n%s\n", recvbuf);
        }
        else if (iResult == 0) {
            printf("Connection closed\n");
        }
        else {
            printf("recv failed: %d\n", WSAGetLastError());
        }
    } while (iResult > 0);

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    /*
    std::cout << "\n\nPress a key to quit.";
    std::cin.get();
    */

    return 0;
}