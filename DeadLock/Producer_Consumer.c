#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define QSIZE 5 // Queue size
#define LOOP 30 // Loop 횟수

typedef struct
{
    int data[QSIZE]; // Queue array
    int index;
    int count;               // # of queue element
    pthread_mutex_t lock;    // mutex
    pthread_cond_t notfull;  // thread condition
    pthread_cond_t notempty; // thread condition
} queue_t;

void *produce(void *args); // thread function
void *consume(void *args); // thread function
void put_data(queue_t *q, int d);
int get_data(queue_t *q);

queue_t *qinit()
{
    queue_t *q;

    q = (queue_t *)malloc(sizeof(queue_t));
    q->index = q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->notfull, NULL);
    pthread_cond_init(&q->notempty, NULL);

    return q;
}

void qdelete(queue_t *q)
{
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->notfull);
    pthread_cond_destroy(&q->notempty);
    free(q);
}

int main()
{
    queue_t *q;
    pthread_t producer, consumer;

    q = qinit();

    pthread_create(&producer, NULL, produce, (void *)q);
    pthread_create(&consumer, NULL, consume, (void *)q);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    qdelete(q);
}

void *produce(void *args)
{
    int i, d;
    queue_t *q = (queue_t *)args;
    for (i = 0; i < LOOP; i++)
    {
        d = i;
        put_data(q, d); // push data
        printf("put data %d to queue\n", d);
    }
    pthread_exit(NULL);
}

void *consume(void *args)
{
    int i, d;
    queue_t *q = (queue_t *)args;
    for (i = 0; i < LOOP; i++)
    {
        d = get_data(q); // pop data
        printf("got data %d from queue\n", d);
    }
    pthread_exit(NULL);
}

void put_data(queue_t *q, int d)
{
    // q->count == QSIZE
    // wait
    pthread_mutex_lock(&(q->lock));
    while (q->count == QSIZE)
    { // wait until q is not full
        pthread_cond_wait(&(q->notfull), &(q->lock));
    }
    q->data[(q->index + q->count) % QSIZE] = d; // push
    q->index %= QSIZE;
    q->count++;
    pthread_cond_signal(&(q->notempty)); // push success, send not empty signal
    pthread_mutex_unlock(&(q->lock));
}

int get_data(queue_t *q)
{
    // q->count == 0
    // wait
    pthread_mutex_lock(&(q->lock));
    while (q->count == 0)
    { // wait until q is not empty
        pthread_cond_wait(&(q->notempty), &(q->lock));
    }
    int ret = q->data[q->index++]; // pop
    q->index %= QSIZE;
    q->count--;
    pthread_cond_signal(&(q->notfull)); // pop success, send not full signal
    pthread_mutex_unlock(&(q->lock));
    return ret;
}