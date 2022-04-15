/**
 * Win32 multi thread udp server example.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code].
 *
 * License - MIT.
 */

#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment(lib, "Ws2_32.lib")


#define DATA_BUFLEN                             512
#define SERVER_IP                               "127.0.0.1"
#define SERVER_PORT                             "65533"


/**
 * StartServer - Start UDP server.
*/
SOCKET StartServer()
{
    int ret;
    WSADATA wsaData;
    SOCKET server_fd = INVALID_SOCKET;

    struct addrinfo *addr_data = NULL;
    struct addrinfo hints;

    /* Initialize Winsock. */
    ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (0 != ret)
    {
        printf("Error in WSAStartup: %d.\n", ret);
        goto out_wsa;
    }

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_DGRAM;
    hints.ai_protocol   = IPPROTO_UDP;
    hints.ai_flags      = AI_PASSIVE;

    /* Resolve the server address and port. */
    ret = getaddrinfo(NULL, SERVER_PORT, &hints, &addr_data);
    if (0 != ret)
    {
        printf("Error in getaddrinfo: %d.\n", ret);
        goto out_getaddr;
    }

    /* Create a SOCKET for connecting to server. */
    server_fd = socket(addr_data->ai_family, addr_data->ai_socktype, addr_data->ai_protocol);
    if (INVALID_SOCKET == server_fd)
    {
        printf("Error in socket: %ld.\n", WSAGetLastError());
        goto out_socket;
    }

    /* Setup the TCP listening socket. */
    ret = bind(server_fd, addr_data->ai_addr, (int)addr_data->ai_addrlen);
    if (SOCKET_ERROR == ret)
    {
        printf("Error in bind: %d.\n", WSAGetLastError());
        goto out_bind;
    }

    freeaddrinfo(addr_data);

    return server_fd;

out_bind:
    closesocket(server_fd);

out_socket:
    freeaddrinfo(addr_data);

out_getaddr:
    WSACleanup();

out_wsa:
    return INVALID_SOCKET;
}

/**
 * Main function.
 */
int main(void)
{
    int status          = 0;
    HANDLE thrdHandle   = NULL;
    SOCKET server_fd    = INVALID_SOCKET;
    char recvbuf[DATA_BUFLEN + 1] = { 0 };

    struct sockaddr_in clt_addr;
    int len = sizeof(clt_addr);

    /* Start TCP Server. */
    server_fd = StartServer();

    if (INVALID_SOCKET == server_fd)
    {
        printf("Start server failed.\n");
        status = -1;
        goto out_end;
    }
    else
    {
        printf("Server startup!\n");
        printf("Server address: %s, Port: %s.\n", SERVER_IP, SERVER_PORT);
        printf("Press CTRL+C to quit.\n\n");
    }

    /* Receive until the peer shuts down the connection. */
    do
    {
        /* Receive data. */
        status = recvfrom(server_fd, recvbuf, DATA_BUFLEN, 0, (struct sockaddr *)&clt_addr, &len);
        if (0 > status)
        {
            printf("Error in recvfrom: %d.\n", WSAGetLastError());
            break;        
        }

        /* Show receive data. */
        recvbuf[status] = 0;
        printf("Client: %s, Received: %s\n", inet_ntoa(clt_addr.sin_addr), recvbuf);

        /* Echo the buffer back to the sender. */
        status = sendto(server_fd, "OK", 2, 0, (struct sockaddr *)&clt_addr, len);

        if (0 > status)
        {
            printf("Error in sendto: %d.\n", WSAGetLastError());
            break;
        }

        Sleep(200);
    } while (1);

    /* Cleanup. */
    closesocket(server_fd);
    WSACleanup();

out_end:
    return status;
}
