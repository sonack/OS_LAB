/* 编译命令:  gcc -o consumer consumer.c -Wall -pthread */
/* Lab6 消费者进程 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h> /* getpid() */
#include <sys/shm.h>

#define MAX_NUM 521
#define CONSUMER_NUM 8
#define BUF_SIZE 10


int main()
{

    sem_t *mutex, *empty, *full;
    int i,
        out,
        data,
        *share,     /* 共享内存地址 */
        share_id;   /* 共享内存的 shmid */

    /* 创建三个信号量 */
    if((mutex = sem_open("MUTEX",O_CREAT,0777,1)) == SEM_FAILED)
    {
        perror("create MUTEX sem failed!\n");
        return -1;
    }

    if((empty = sem_open("EMPTY",O_CREAT,0777,BUF_SIZE)) == SEM_FAILED)
    {
        perror("create EMPTY sem failed!\n");
        return -1;
    }

    if((full = sem_open("FULL",O_CREAT,0777,0)) == SEM_FAILED)
    {
        perror("create FULL sem failed!\n");
        return -1;
    }

     /* 建立共享内存 */
    share_id = shmget(23333, sizeof(int) * BUF_SIZE, IPC_CREAT | 0666);

    if(share_id == -1)
    {
        perror("consumer shmget failed!\n");
        return -1;
    }

    share = (int *)shmat(share_id, NULL, 0);

    if(share == (int *)0xffffffff)
    {
        perror("consumer shmat failed!\n");
        return -1;
    }

    for(i = 0; i < MAX_NUM; i++)
    {
        sem_wait(full);
        sem_wait(mutex);

        /*消费资源*/
        data = share[out];
        out = (out + 1) % BUF_SIZE;
    
        printf("%d:  %d\n",getpid(),data);
        fflush(stdout);

        sem_post(mutex);
        sem_post(empty);
    }

    sem_unlink("MUTEX");
    sem_unlink("EMPTY");
    sem_unlink("FULL");

    return 0;
}