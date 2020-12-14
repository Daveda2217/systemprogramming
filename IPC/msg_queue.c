#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include <time.h>

struct msg
{
    long msgtype;
    char text[256];
};

struct msg_ack
{
    long msgtype;
    char text[256];
};

int main()
{
    int uid;
    int receiver_id;
    printf("my id is\n");
    scanf("%d", &uid);
    getchar(); // flush
    printf("who to send?\n");
    scanf("%d", &receiver_id);
    getchar(); // flush

    key_t k = ftok(".", 'a');
    int qid = msgget(k, IPC_CREAT | 0660);

    if (qid == -1)
    {
        perror("msgget error : ");
        exit(0);
    }

    //struct msg* msg_send = malloc(sizeof(struct msg));
    //struct msg_ack* msg_recv = malloc(sizeof(struct msg_ack));

    struct msg msg_send;

    msg_send.msgtype = uid;

    time_t t;
    if (fork() == 0)
    {
        while (1)
        {
            /* receive message  */
            /* get msg from queue and print out */

            if (msgrcv(qid, (void *)&msg_send, sizeof(struct msg), receiver_id, IPC_NOWAIT) > 0)
            {
                printf("RECEIVED %s\n\n", msg_send.text);

                t = time(NULL);
                msg_send.msgtype = uid + 10;
                msgsnd(qid, (void *)&msg_send, sizeof(struct msg), 0); // send time
                if (msgrcv(qid, (void *)&msg_send, sizeof(struct msg), uid + 10, IPC_NOWAIT) > 0)
                {
                    // uid + 10 get msg
                    strcpy(msg_send.text, ctime(&t));
                    printf("%d read message at:\t%s\n", receiver_id, msg_send.text);
                }
            }

            //	if(msgrcv(qid,(void*)&msg_send,sizeof(struct msg),uid+10,IPC_NOWAIT)>0){
            //		strcpy(msg_send.text, ctime(&t));
            //		printf("%d read message at:\t%s\n", receiver_id,msg_send.text);
            //	}
        }
    }
    else
    {
        while (1)
        {
            /* send message  */
            /* get user input and send to the msg queue*/
            gets(msg_send.text);
            if (msgsnd(qid, (void *)&msg_send, sizeof(struct msg), 0) == -1)
            {
                perror("msgsnd error : ");
                exit(0);
            }
        }
    }

    //free(msg_send);
    //free(msg_recv);

    return 0;
}
