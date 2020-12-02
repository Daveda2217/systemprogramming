#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAXNAME 10 // 최대 User 이름 길이
#define MAXLINE 80 // 최대 메세지 길이

int main(int argc, char *argv[])
{

    int n, cfd, fdmax, fdnum;
    struct hostent *h;
    struct sockaddr_in saddr;
    char buf[MAXLINE];
    char name[MAXNAME];
    char temp[MAXLINE + MAXNAME + 1];
    char *host = argv[1];
    int port = atoi(argv[2]);
    fd_set readset, copyset;

    printf("Insert your name : ");
    scanf("%s", name); // User 이름 받기.

    if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    { // 소켓 생성하기.
        printf("socket() failed.\n");
        exit(1);
    }

    if ((h = gethostbyname(host)) == NULL)
    { // 호스트 이름 받기.
        printf("invalid hostname %s\n", host);
        exit(2);
    }

    // 소켓 기본 설정하기.
    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *)&saddr.sin_addr.s_addr, (char *)h->h_addr, h->h_length);
    saddr.sin_port = htons(port);

    // 소켓에 연결하기.
    if (connect(cfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        printf("connect() failed.\n");
        exit(3);
    }

    /* Insert your code for file descriptor set*/
    FD_ZERO(&readset);         // fd_set 초기화를 해준다.
    FD_SET(cfd, &readset);     // cfd는 연결했으니 fd_set을 해준다.
    FD_SET(0, &readset);       // stdin도 fd_set을 해준다.
    fdmax = cfd;               // 가장 큰 fdmax는 cfd이다.
    write(cfd, name, MAXNAME); // 일단 User의 이름을 cfd에 보내준다. (server에 전송)

    while (1)
    {

        memset(buf, 0, MAXLINE);                // buf 초기화를 해준다.
        memset(temp, 0, MAXLINE + MAXNAME + 1); // temp 초기화를 해준다.

        copyset = readset; // readset을 복사해준다.

        if ((fdnum = select(fdmax + 1, &copyset, NULL, NULL, NULL)) < 0)
        { // select함수를 이용해 반환 값을 fdnum에 넣어준다.
            printf("select() failed.\n");
            exit(4);
        }

        /* Insert your code */
        if (FD_ISSET(0, &copyset)) // 만약 stdin이 set되어 있다면
        {
            if ((n = read(0, buf, MAXLINE)) > 0) // buf에 stdin의 값을 저장한다.
            {
                if (!strcmp(buf, "quit\n")) // quit이면 바로 종료하고
                    break;
                write(cfd, buf, n); // cfd 즉, 서버에 전송해준다.
            }
        }

        if (FD_ISSET(cfd, &copyset)) // 서버에서 전송한 메세지가 있다면
        {
            if ((n = read(cfd, temp, MAXLINE + MAXNAME + 1)) > 0) // temp에 저장해주고
            {
                printf("%s\n", temp); // 해당 메세지를 출력한다.
            }
        }
    }
    return 0;
}
