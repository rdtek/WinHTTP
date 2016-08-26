#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 512

//MSDN Winsock Client Application
//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530750(v=vs.85).aspx

int main(int argc, char* argv[]) {

    std::cout << "SOCKET TEST";

    WSADATA wsaData;

    int iResult;

    //==========================================================================
    // Initialize Winsock - initiate use of WS2_32.dll.
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
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("\ngetaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;

    //==========================================================================
    // Attempt to connect to the first address returned by getaddrinfo
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
    // Send an initial buffer
    //==========================================================================
    int recvbuflen = DEFAULT_BUFLEN;

    char *sendbuf = "GET / HTTP/1.0\r\nHOST: www.bbc.com\r\n\r\n";
    char recvbuf[DEFAULT_BUFLEN];

    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("\nBytes Sent: %ld\n", iResult);
    if (iResult > 0) {
        printf("\n=== Sent Message: ===\n%s\n", sendbuf);
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
    // Receive data until the server closes the connection
    //==========================================================================
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            printf("\n=== Received Message ===\n%s\n", recvbuf);
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

    std::cout << "\n\nPress a key to quit.";
    //std::cin.get();

    return 0;
}