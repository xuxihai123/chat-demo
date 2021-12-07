/*
 * tcp-echo-client.c
 */

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
    if (argc != 3) {
        fprintf(stderr, "usage: %s <server-ip> <server-port>\n",
                argv[0]);
        exit(1);
    }

    const char *ip = argv[1];
    unsigned short port = atoi(argv[2]);

    // Create a socket for TCP connection

    int sock; // socket descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("socket failed");

    // Construct a server address structure

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr)); // must zero out the structure
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port        = htons(port); // must be in network byte order

    // Establish a TCP connection to the server

    if (connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        die("connect failed");

    // Read a line from stdin and send it to the server

    char buf[100];
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        die("fgets returned NULL");
    }
    size_t len = strlen(buf);

    /*
     * send(int socket, const void *buffer, size_t length, int flags)
     *
     *   - normally, send() blocks until it sends all bytes requested
     *   - returns num bytes sent or -1 for error
     *   - send(sock,buf,len,0) is equivalent to write(sock,buf,len)
     */
    if (send(sock, buf, len, 0) != len)
        die("send failed");

    // Receive the responses from the server and print them

    /*
     * recv(int socket, void *buffer, size_t length, int flags)
     *
     *   - normally, recv() blocks until it has received at least 1 byte
     *   - returns num bytes received, 0 if connection closed, -1 if error
     *   - recv(sock,buf,len,0) is equivalent to read(sock,buf,len)
     */
    int r;
    while (len > 0 && (r = recv(sock, buf, sizeof(buf), 0)) > 0) {
        printf("RECEIVED: [");
        fwrite(buf, 1, r, stdout);
        printf("]\n");
        len -= r;
    }
    if (r < 0)
        die("recv failed");

    // Clean-up

    close(sock);
    return 0;
}