#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXUSER 10 // 최대 User 수
#define MAXNAME 10 // 최대 User 이름의 길이
#define MAXLINE 80 // 최대 메세지 길이

int main(int argc, char *argv[])
{

    int n, listenfd, connfd, caddrlen, fdmax, fdnum, usernum;
    struct hostent *h;
    struct sockaddr_in saddr, caddr;
    char buf[MAXLINE];
    char name[MAXUSER][MAXNAME];
    char temp[MAXLINE + MAXNAME + 1];
    int port = atoi(argv[1]);
    fd_set readset, copyset;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    { // 가장 먼저 socket을 생성하자
        printf("socket() failed.\n");
        exit(1);
    }

    // socket 초기 설정하기
    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    // bind함수를 호출해주고
    if (bind(listenfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        printf("bind() failed.\n");
        exit(2);
    }

    // listen함수를 통해 클라이언트를 기다린다.
    if (listen(listenfd, 5) < 0)
    {
        printf("listen() failed.\n");
        exit(3);
    }

    /* Insert your code for file descriptor set*/
    FD_ZERO(&readset);          // fdset 또한 초기화 시켜준다.
    FD_SET(listenfd, &readset); // listenfd값을 setting 해주고
    fdmax = listenfd;           // fdmax값을 선언해주자.
    usernum = 0;                // 현재 User 수는 0명이다.

    while (1)
    {

        copyset = readset; // copyset에 현재 readset을 복사하자.

        if ((fdnum = select(fdmax + 1, &copyset, NULL, NULL, NULL)) < 0) // select함수를 통해 반환 값을 fdnum에 저장한다.
        {
            printf("select() failed.\n");
            exit(4);
        }

        for (int i = 3; i < fdmax + 1; i++) // stdin, stdout, stderr를 제외한 fd를 순환한다.
        {

            memset(buf, 0, MAXLINE);                // buf를 초기화 시켜준다.
            memset(temp, 0, MAXLINE + MAXNAME + 1); // temp을 초기화 시켜준다.

            if (FD_ISSET(i, &copyset)) // 만약 fd i값이 set되었고
            {

                if (i == listenfd) // 이 값이 listenfd라면
                {

                    caddrlen = sizeof(caddr); // accept를 해준다.
                    if ((connfd = accept(listenfd, (struct sockaddr *)&caddr, (socklen_t *)&caddrlen)) < 0)
                    {
                        printf("accept() failed.\n");
                        continue;
                    }

                    /* Insert your code */
                    FD_SET(connfd, &readset); // accept함수의 반환 값인 connfd를 set시켜주고
                    usernum++;                // user 수를 증가시켜주자.
                    if (fdmax < connfd)       // 만약 fdmax보다 connfd가 더 크다면 update 해준다.
                        fdmax = connfd;
                    if ((n = read(connfd, name[connfd], MAXNAME)) > 0) // name[connfd]에 User 이름을 저장해주고
                    {
                        for (int j = 3; j < fdmax + 1; j++) // 다른 클라이언트들에게 join했다는 메세지를 보내준다.
                        {
                            if (i != j && j != connfd) // j(순회 index)는 i(즉, listenfd)가 아니면서 자기 자신이 아니어야 한다.
                            {
                                sprintf(temp, "%s joined. %d current users", name[connfd], usernum); // temp에 메세지를 저장해주고
                                write(j, temp, MAXLINE + MAXNAME + 1);                               // 클라이언트에 전송한다.
                            }
                        }
                    }
                }
                else
                {
                    /* Insert your code */
                    if ((n = read(i, buf, MAXLINE)) > 0) // 만약 i가 listenfd가 아니라면
                    {
                        printf("got %d bytes from user %s.\n", n, name[i]); // 클라이언트로부터 전송된 메세지를 받는다.
                        for (int j = 3; j < fdmax + 1; j++)                 // 받은 메세지를 다른 클라이언트들에게 다시 전송한다.
                        {
                            if (j != listenfd && i != j)
                            {
                                sprintf(temp, "%s:%s", name[i], buf);
                                write(j, temp, MAXLINE + MAXNAME + 1);
                            }
                        }
                    }
                    else
                    {
                        FD_CLR(i, &readset);                // 더 이상 전송되는 메세지가 없다면
                        usernum--;                          // User 수를 감소시켜주고
                        for (int j = 3; j < fdmax + 1; j++) // 다른 클라이언트들에게 나갔다는 메세지를 전송한다.
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
