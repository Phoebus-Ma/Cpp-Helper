/**
 * Win32 multi thread udp client example.
 * Ref: [https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code].
 *
 * License - MIT.
 */

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")


#define DATA_BUFLEN                             512
#define SERVER_IP                               "127.0.0.1"
#define SERVER_PORT                             65533


/**
 * ConnectServer - Connect udp server.
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

    /* Create a SOCKET for connecting to server. */
    client_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (INVALID_SOCKET == client_fd)
    {
        printf("Error in socket: %ld.\n", WSAGetLastError());
        goto out_wsa;
    }

    return client_fd;

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

    struct sockaddr_in ser_addr;
    int len = sizeof(ser_addr);

    /* Connect to server. */
    client_fd = ConnectServer();

    if (INVALID_SOCKET == client_fd)
    {
        printf("Error in connect server.\n");
        status = -1;
        goto out_end;
    }
    
    printf("Connected Server!\n");

    ser_addr.sin_family             = AF_INET;
    ser_addr.sin_port               = htons(SERVER_PORT);
    ser_addr.sin_addr.S_un.S_addr   = inet_addr(SERVER_IP);

    /* Receive until the peer closes the connection. */
    for (int i = 0; i < 5; i++)
    {
        /* Send an initial buffer. */
        ret = sendto(client_fd, sendbuf, (int)strlen(sendbuf), 0, (struct sockaddr *)&ser_addr, len);

        if (0 > ret)
        {
            printf("Error in send: %d.\n", WSAGetLastError());
            status = -1;
            break;
        }

        ret = recvfrom(client_fd, recvbuf, DATA_BUFLEN, 0, (struct sockaddr *)&ser_addr, &len);

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

    /* cleanup. */
    closesocket(client_fd);
    WSACleanup();

out_end:
    return status;
}
