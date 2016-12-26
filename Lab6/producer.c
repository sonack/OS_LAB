/* 编译命令:  gcc -o producer producer.c -Wall -pthread */

/** Lab6 生产者进程 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/shm.h>

#define MAX_NUM 521
#define CONSUMER_NUM 8
#define BUF_SIZE 10

int main()
{

    sem_t *mutex, *empty, *full;
    int i,
        in,
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
        perror("producer shmget failed!\n");
        return -1;
    }

    share = (int *)shmat(share_id, NULL, 0);

    if(share == (int *)0xffffffff)
    {
        perror("producer shmat failed!\n");
        return -1;
    }

    for(i = 0; i < MAX_NUM; i++)
    {
        sem_wait(empty);
        sem_wait(mutex);
        share[in] = i;
        in = (in + 1) % BUF_SIZE;
        sem_post(mutex);
        sem_post(full);
    }

    sem_unlink("MUTEX");
    sem_unlink("EMPTY");
    sem_unlink("FULL");

    return 0;
}