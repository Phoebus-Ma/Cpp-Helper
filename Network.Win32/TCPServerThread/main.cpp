/**
 * Win32 multi thread tcp server example.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code].
 *
 * License - MIT.
 */

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment(lib, "Ws2_32.lib")


#define DATA_BUFLEN                             512
#define SERVER_IP                               "127.0.0.1"
#define SERVER_PORT                             "65533"


/**
 * ClientHandler - TCP Client Thread working function.
*/
DWORD WINAPI
ClientHandler(LPVOID lpParam)
{
    int ret                         = -1;
    int status                      = 0;
    char recvbuf[DATA_BUFLEN + 1]   = { 0 };
    SOCKET client_fd                = *(SOCKET *)lpParam;

    /* Receive until the peer shuts down the connection. */
    do
    {
        ret = recv(client_fd, recvbuf, DATA_BUFLEN, 0);
        if (0 < ret)
        {
            /* Echo the buffer back to the sender. */
            if (SOCKET_ERROR == send(client_fd, "OK", 2, 0))
            {
                printf("Error in send: %d.\n", WSAGetLastError());
                status = -1;
                break;
            }

            /* Show receive data. */
            recvbuf[ret] = 0;
            printf("Received: %s\n", recvbuf);
        }
        else if (0 == ret)
        {
            printf("Client %lld will close...\n", client_fd);
            break;
        }
        else
        {
            printf("Error in recv: %d.\n", WSAGetLastError());
            status = -1;
            break;
        }
    } while (0 < ret);

    /* shutdown the connection since we're done. */
    ret = shutdown(client_fd, SD_BOTH);
    if (SOCKET_ERROR == ret)
    {
        printf("Error in shutdown: %d.\n", WSAGetLastError());
        status = -1;
    }

    closesocket(client_fd);
    printf("Client %lld closed.\n", client_fd);

    return status;
}

/**
 * StartServer - Start TCP server.
*/
SOCKET StartServer(void)
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
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;
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

    ret = listen(server_fd, SOMAXCONN);
    if (SOCKET_ERROR == ret)
    {
        printf("Error in listen: %d.\n", WSAGetLastError());
        goto out_listen;
    }

    return server_fd;

out_listen:
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
    SOCKET client_fd    = INVALID_SOCKET;

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
        printf("Press CTRL+C to quit.\n");
    }

    /* Waitting client. */
    while (TRUE)
    {
        /* Accept a client socket. */
        client_fd = accept(server_fd, NULL, NULL);

        if (INVALID_SOCKET == client_fd)
        {
            printf("Error in accept: %d.\n", WSAGetLastError());
            continue;
        }
        else
        {
            printf("Connect client: %lld.\n", client_fd);

            /* Create working thread. */
            thrdHandle = CreateThread(
                NULL,
                0,
                ClientHandler,
                &client_fd,
                0,
                NULL
            );

            if (NULL == thrdHandle)
            {
                printf("Error in CreateThread from client: %lld.\n", client_fd);
                continue;
            }
        }

        Sleep(1000);
    }

    /* Cleanup. */
    closesocket(server_fd);
    WSACleanup();

out_end:
    return status;
}
