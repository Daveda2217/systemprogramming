#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAXLINE 50

int main(int argc, char *argv[])
{
    int n, cfd;
    struct hostent *h;
    struct sockaddr_in saddr;
    char buf[MAXLINE];
    char *host = argv[1];
    int port = atoi(argv[2]);

    if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() failed.\n");
        exit(1);
    }

    if ((h = gethostbyname(host)) == NULL)
    {
        printf("invalid hostname %s\n", host);
        exit(2);
    }

    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *)&saddr.sin_addr.s_addr, (char *)h->h_addr, h->h_length);
    saddr.sin_port = htons(port);

    if (connect(cfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        printf("connect() failed.\n");
        exit(3);
    }

    while (1)
    {
        memset(buf, 0, MAXLINE);
        int total = 0;
        n = read(0, buf, MAXLINE);
        buf[n - 1] = '\0';

        write(cfd, buf, n); // send file name

        if (strcmp(buf, "quit") == 0)
            break;

        int fd = open(buf, O_RDONLY);
        char len[MAXLINE];
        char buf_cnt[200000];

        while ((n = read(fd, buf, 1)) > 0) // count total bytes & store content
        {
            buf_cnt[total++] = buf[0];
        }
        sprintf(len, "%d", total);
        write(cfd, len, MAXLINE); // send total bytes
        buf_cnt[total] = '\0';

        printf("Send %d bytes to server.\n", total);
        write(cfd, buf_cnt, total); // send content
        close(fd);
    }
    close(cfd);

    return 0;
}
