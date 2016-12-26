/* Lab6 生产者进程 linux-0.11 */
#define __LIBRARY__  /*使用系统调用*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

_syscall2(sem_t *, sem_open,const char *, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t *, sem);
_syscall1(int, sem_post, sem_t *, sem);
_syscall1(int, sem_unlink, const char *, name);
_syscall1(int, shmget, int, key);
_syscall1(void *, shmat, int, shmid);


#define MAX_NUM 521
#define BUF_SIZE 10

int main()
{

    sem_t *mutex, *empty, *full;
    int i,
        in = 0,
        *share,     /* 共享内存地址 */
        share_id;   /* 共享内存的 shmid */

    /* 创建三个信号量 */
    if((mutex = sem_open("MUTEX",1)) == SEM_FAILED)
    {
        perror("create MUTEX sem failed!\n");
        return -1;
    }

    if((empty = sem_open("EMPTY",BUF_SIZE)) == SEM_FAILED)
    {
        perror("create EMPTY sem failed!\n");
        return -1;
    }

    if((full = sem_open("FULL",0)) == SEM_FAILED)
    {
        perror("create FULL sem failed!\n");
        return -1;
    }

    /* 建立共享内存 */
    share_id = shmget(23333);

    if(share_id == -1)
    {
        perror("producer shmget failed!\n");
        return -1;
    }

    share = (int *)shmat(share_id);

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

    return 0;
}