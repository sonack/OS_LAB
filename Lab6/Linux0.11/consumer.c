/* Lab6 消费者进程 linux-0.11*/
#define __LIBRARY__  /*使用系统调用*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* getpid() */

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
    	out = 0,
    	data,
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
        perror("consumer shmget failed!\n");
        return -1;
    }

    share = (int *)shmat(share_id);

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
        data = share[out % BUF_SIZE];
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