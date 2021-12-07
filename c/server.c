/*
 * tcp-echo-server.c
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

static void die(const char *s) { perror(s); exit(1); }

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <server-port>\n", argv[0]);
        exit(1);
    }

    unsigned short port = atoi(argv[1]);

    // Create a listening socket (also called server socket) 

    int servsock;
    if ((servsock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("socket failed");

    // Construct local address structure

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // any network interface
    servaddr.sin_port = htons(port);

    // Bind to the local address

    if (bind(servsock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        die("bind failed");

    // Start listening for incoming connections

    if (listen(servsock, 5 /* queue size for connection requests */ ) < 0)
        die("listen failed");

    int r;
    char buf[10];

    int clntsock;
    socklen_t clntlen;
    struct sockaddr_in clntaddr;

    while (1) {

        // Accept an incoming connection

        fprintf(stderr, "waiting for client ... ");

        clntlen = sizeof(clntaddr); // initialize the in-out parameter

        if ((clntsock = accept(servsock,
                        (struct sockaddr *) &clntaddr, &clntlen)) < 0)
            die("accept failed");

        // accept() returned a connected socket (also called client socket)
        // and filled in the client's address into clntaddr

        fprintf(stderr, "client ip: %s\n", inet_ntoa(clntaddr.sin_addr));

        // Receive msg from client, capitalize the 1st char, send it back

        while ((r = recv(clntsock, buf, sizeof(buf), 0)) > 0) {
            *buf = toupper(*buf);
            if (send(clntsock, buf, r, 0) != r) {
                fprintf(stderr, "ERR: send failed\n");
                break;
            }
        }
        if (r < 0) {
            fprintf(stderr, "ERR: recv failed\n");
        }

        // Client closed the connection (r==0) or there was an error
        // Either way, close the client socket and go back to accept()

        close(clntsock);
    }
}