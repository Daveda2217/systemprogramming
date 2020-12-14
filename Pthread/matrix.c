#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct thread_data
{
    int thread_id;
    int result;
};

int row_size;
int col_size;

int **matrix;
int *vector;

void print_matrix()
{
    printf(" *** Matrix ***\n");
    for (int i = 0; i < row_size; i++)
    {
        for (int j = 0; j < col_size; j++)
        {
            printf("[ %d ] ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_vector()
{
    printf(" *** Vector ***\n");
    for (int i = 0; i < col_size; i++)
    {
        printf("[ %d ]\n", vector[i]);
    }
}

void *thread_mvm(void *arg)
{
    int res = 0;

    for (int i = 0; i < col_size; i++)
    {
        res += matrix[(struct thread_data)arg->thread_id][i] * vector[i];
    }

    (struct thread_data) arg->result = res;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <row> <column>\n", argv[0]);
        exit(1);
    }

    row_size = atoi(argv[1]);
    col_size = atoi(argv[2]);
    pthread_t tid[row_size];
    struct thread_data t_data[row_size];

    srand(time(NULL));

    matrix = (int **)malloc(sizeof(int *) * row_size);

    for (int i = 0; i < row_size; i++)
    {
        matrix[i] = (int *)malloc(sizeof(int) * col_size);
    }

    vector = (int *)malloc(sizeof(int) * col_size);
    for (int i = 0; i < row_size; i++)
    {
        for (int j = 0; j < col_size; j++)
        {
            matrix[i][j] = rand() % 10;
        }
    }

    for (int i = 0; i < col_size; i++)
    {
        vector[i] = rand() % 10;
    }

    print_matrix();
    print_vector();

    for (int i = 0; i < row_size; i++)
    {
        t_data[i].thread_id = i;
        pthread_create(&tid[i], NULL, thread_mvm, (void *)t_data[i]);
    }

    for (int i = 0; i < row_size; i++)
    {
        pthread_join(tid[i], NULL);
    }

    printf("\n *** Result ***\n");
    for (int i = 0; i < row_size; i++)
    {
        printf("[ %d ]\n", t_data[i].result);
    }
    for (int i = 0; i < row_size; i++)
    {
        free(matrix[i]);
    }

    free(matrix);
    free(vector);
    return 0;
}