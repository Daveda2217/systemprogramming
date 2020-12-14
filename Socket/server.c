#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define MAXLINE 50

int main(int argc, char *argv[])
{
    int n, listenfd, connfd, caddrlen;
    struct hostent *h;
    struct sockaddr_in saddr, caddr;
    char buf[MAXLINE];
    int port = atoi(argv[1]);

    if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() failed.\n");
        exit(1);
    }

    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        printf("bind() failed.\n");
        exit(2);
    }

    if (listen(listenfd, 5) < 0)
    {
        printf("listen() failed.\n");
        exit(3);
    }

    caddrlen = sizeof(caddr);

    if ((connfd = accept(listenfd, (struct sockaddr *)&caddr, &caddrlen)) < 0)
    {
        printf("accept() failed.\n");
        exit(4);
    }

    while (1)
    {

        int total = 0;
        memset(buf, 0, MAXLINE);

        n = read(connfd, buf, MAXLINE); // Receive name of file
        buf[n - 1] = '\0';

        if (strcmp(buf, "quit") == 0)
            break;
        printf("FILE NAME: %s\n", buf);

        char buf_name[MAXLINE + 6];
        sprintf(buf_name, "%s_copy", buf);

        memset(buf, 0, MAXLINE);
        n = read(connfd, buf, MAXLINE); // get total bytes
        buf[n - 1] = '\0';

        total = atoi(buf);

        printf("got %d bytes from client.\n", total);

        char buf_cnt[200000];
        int fd = open(buf_name, O_CREAT | O_RDWR, 0644);
        n = read(connfd, buf_cnt, total); // get content
        buf_cnt[n - 1] = '\0';
        write(fd, buf_cnt, total);
        close(fd);
    }
    close(connfd);

    return 0;
}