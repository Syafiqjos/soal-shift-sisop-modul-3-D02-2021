#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define x 4
#define y 3
#define z 6

// const int matrix1[x][y] = {{1, 2, 3}, {4, 5, 1}, {2, 3, 4}, {5, 1, 2}};
// const int matrix2[y][z] = {{1, 2, 3, 4, 5, 1}, {2, 3, 4, 5, 1, 2}, {3, 4, 5, 1, 2, 3}};
int matrix1[x][y];
int matrix2[y][z];
int result[x][z];

pthread_t tid[x*z];
pthread_attr_t attr;
int iret[x*z];
key_t key = 1234;
int *matrix;
int cnt=0;

void *hitung(void *arguments);

struct arg_struct{
    int arg1;
    int arg2;
};

void call_thread (){
    for(int i=1; i<x+1; i++){
        for(int j=1; j<z+1; j++){
            struct arg_struct *args = (struct arg_struct *) malloc(sizeof(struct arg_struct));
            args->arg1 = i-1;
            args->arg2 = j-1;
            pthread_attr_init(&attr);
            iret[cnt] = pthread_create(&tid[cnt], &attr, hitung, args);
            if(iret[cnt]){
                fprintf(stderr,"Error - pthread_create() return code: %d\n", iret[cnt]);
                exit(EXIT_FAILURE);
            }
            cnt++;
        }
    }
}

void join_thread(){
    for(int i=0; i<x; i++){
        for(int j=0; j<z; j++){
            pthread_join(tid[cnt], NULL);
            printf("%d\t", result[i][j]);
            matrix[cnt] = result[i][j];
            cnt++;
        }
        printf("\n");
    }
}

int main(void){
    printf("Input Matrix 1\n");
    for (int i = 0; i < x; ++i) {
      for (int j = 0; j < y; ++j) {
         printf("Enter a%d%d: ", i + 1, j + 1);
         scanf("%d", &matrix1[i][j]);
      }
    }

    printf("Input Matrix 2\n");
    for (int i = 0; i < y; ++i) {
      for (int j = 0; j < z; ++j) {
         printf("Enter a%d%d: ", i + 1, j + 1);
         scanf("%d", &matrix2[i][j]);
      }
    }

    printf("\n");

    int shmid = shmget(key, sizeof(matrix), IPC_CREAT | 0666);
    matrix = shmat(shmid, 0, 0);

    call_thread();

    cnt=0;
    join_thread();

    shmdt(matrix);

    return 0;
}

void *hitung(void *arguments){
    struct arg_struct *args = arguments;

    int temp = 0;
    int d1=args->arg1;
    int d2=args->arg2;

    for(int i=0; i<y; i++){
        temp = temp + (matrix1[d1][i] * matrix2[i][d2]);
    }

    result[d1][d2] = temp;
    pthread_exit(0);
}
