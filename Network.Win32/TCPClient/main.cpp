/**
 * Win32 multi thread tcp client example.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code].
 *
 * License - MIT.
 */

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")


#define DATA_BUFLEN                             512
#define SERVER_IP                               "127.0.0.1"
#define SERVER_PORT                             "65533"


/**
 * ConnectServer - Connect tcp server.
*/
SOCKET ConnectServer()
{
    int ret = -1;
    WSADATA wsaData;
    SOCKET client_fd = INVALID_SOCKET;

    struct addrinfo *addr_data = NULL,
                    *addr_ptr  = NULL,
                    hints;

    /* Initialize Winsock. */
    ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (0 != ret)
    {
        printf("Error in WSAStartup: %d.\n", ret);
        goto out_wsa;
    }

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;

    /* Resolve the server address and port. */
    ret = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &addr_data);

    if (0 != ret)
    {
        printf("Error in getaddrinfo: %d.\n", ret);
        goto out_wsa;
    }

    /* Attempt to connect to an address until one succeeds. */
    for (addr_ptr = addr_data; addr_ptr != NULL; addr_ptr = addr_ptr->ai_next)
    {
        /* Create a SOCKET for connecting to server. */
        client_fd = socket(
            addr_ptr->ai_family,
            addr_ptr->ai_socktype,
            addr_ptr->ai_protocol
        );

        if (INVALID_SOCKET == client_fd)
        {
            printf("Error in socket: %ld.\n", WSAGetLastError());
            goto out_sock;
        }

        /* Connect to server. */
        ret = connect(client_fd, addr_ptr->ai_addr, (int)addr_ptr->ai_addrlen);

        if (SOCKET_ERROR == ret)
        {
            closesocket(client_fd);
            client_fd = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo(addr_data);

    if (INVALID_SOCKET == client_fd)
    {
        printf("Unable to connect to server!\n");
    }

    return client_fd;

out_sock:
    freeaddrinfo(addr_data);

out_wsa:
    WSACleanup();

    return INVALID_SOCKET;
}

/**
 * Main function.
 */
int main(void)
{
    int ret             = -1;
    int status          = 0;
    SOCKET client_fd    = INVALID_SOCKET;
    const char *sendbuf = "Hello, 0123456789.";

    char recvbuf[DATA_BUFLEN + 1] = { 0 };

    /* Connect to server. */
    client_fd = ConnectServer();

    if (INVALID_SOCKET == client_fd)
    {
        printf("Error in connect server.\n");
        status = -1;
        goto out_end;
    }
    
    printf("Connected Server!\n");

    /* Receive until the peer closes the connection. */
    for (int i = 0; i < 5; i++)
    {
        /* Send an initial buffer. */
        ret = send(client_fd, sendbuf, (int)strlen(sendbuf), 0);

        if (SOCKET_ERROR == ret)
        {
            printf("Error in send: %d.\n", WSAGetLastError());
            status = -1;
            break;
        }

        ret = recv(client_fd, recvbuf, DATA_BUFLEN, 0);

        if (0 < ret)
        {
            recvbuf[ret] = 0;
            printf("Received: %s.\n", recvbuf);
        }
        else if (0 == ret)
        {
            printf("Connection closed.\n");
            break;
        }
        else
        {
            printf("Error in recv: %d.\n", WSAGetLastError());
        }

        Sleep(1000);
    }

    /* shutdown the connection since no more data will be sent. */
    ret = shutdown(client_fd, SD_BOTH);

    if (SOCKET_ERROR == ret)
    {
        printf("Error in shutdown: %d.\n", WSAGetLastError());
        status = -1;
    }

    /* cleanup. */
    closesocket(client_fd);
    WSACleanup();

out_end:
    return status;
}
