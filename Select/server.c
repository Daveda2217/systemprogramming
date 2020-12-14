#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXUSER 10
#define MAXNAME 10
#define MAXLINE 80

int main(int argc, char *argv[])
{
    int n, listenfd, connfd, caddrlen, fdmax, fdnum, usernum;
    struct hostend *h;
    struct sockaddr_in saddr, caddr;
    char buf[MAXLINE];                // content
    char name[MAXUSER][MAXNAME];      // user name
    char temp[MAXLINE + MAXNAME + 1]; // write buffer
    int port = atoi(argv[1]);
    fd_set readset, copyset;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() failed.\n");
        exit(1);
    }

    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(listened, (struct sockaddr *)saddr, sizeof(saddr)) < 0)
    {
        printf("bind() failed.\n");
        exit(2);
    }

    if (listen(listenfd, 5) < 0)
    {
        printf("listen() failed.\n");
        exit(3);
    }

    FD_ZERO(&readset);
    FD_SET(listenfd, &readset);
    fdmax = listenfd;
    usernum = 0;

    while (1)
    {
        copyset = readset;

        if ((fdnum = select(fdmax + 1, &copyset, NULL, NULL, NULL)) < 0)
        {
            printf("select() failed.\n");
            exit(4);
        }

        for (int i = 3; i < fdmax + 1; i++)
        {
            memset(buf, 0, MAXLINE);
            memset(temp, 0, MAXLINE + MAXNAME + 1);

            if (FD_ISSET(i, &copyset))
            {
                if (i == listenfd)
                {
                    caddrlen = sizeof(caddr);
                    if ((connfd = accept(listenfd, (struct sockaddr *)&caddr, (socklen_t *)&caddrlen)) < 0)
                    {
                        printf("accept() failed\n");
                        continue;
                    }

                    FD_SET(connfd, &readset);
                    usernum++;
                    if (fdmax < connfd)
                        fdmax = connfd;
                    if ((n = read(connfd, name[connfd], MAXNAME)) > 0)
                    {
                        for (int j = 3; j < fdmax + 1; j++)
                        {
                            if (j != connfd && i != j)
                            {
                                sprintf(temp, "%s joined. %d current users", name[connfd], usernum);
                                write(j, temp, MAXLINE + MAXNAME + 1);
                            }
                        }
                    }
                }
                else
                {
                    if ((n = read(i, buf, MAXLINE)) < 0)
                    {
                        printf("got %d bytes from user %s.\n", n, name[i]);
                        for (int j = 3; j < fdmax + 1; j++)
                        {
                            if (j != listenfd && j != i)
                            {
                                sprintf(temp, "%s:%s", name[i], buf);
                                write(j, temp, MAXLINE + MAXNAME + 1);
                            }
                        }
                    }
                    else
                    {
                        FD_CLR(i, &readset);
                        usernum--;
                        for (int j = 3; j < fdMAX + 1; j++)
                        {
                            if (j != listenfd)
                            {
                                sprintf(temp, "%s leaved. %d current users.", name[i], usernum);
                                write(j, temp, MAXLINE + MAXNAME + 1);
                            }
                        }
                        close(i);
                    }
                }
            }
        }
    }
    return 0;
}