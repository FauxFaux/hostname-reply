#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

    const char *head = "HTTP/1.0 200\r\nContent-type: text/plain\r\n\r\n";
    char buf[256] = {};
    memcpy(buf, head, strlen(head));
    if (0 != gethostname(buf + strlen(buf), sizeof(buf))) {
        return 4;
    }

    strcat(buf, "\n");

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

#ifdef REUSEADDR
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        return 5;
    }
#endif

    struct sockaddr_in servaddr = {};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1337);
    if (0 != bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) {
        return 1;
    }

    if (0 != listen(sockfd, 50)) {
        return 2;
    }

    for (;;) {
        int conn = accept(sockfd, NULL, NULL);
        if (conn < 0) {
            return 3;
        }

        // errors intentionally ignored
        write(conn, buf, strlen(buf));

        // without shutdown, curl sometimes gets the close before the data
        shutdown(conn, SHUT_RDWR);
        close(conn);
    }
}

