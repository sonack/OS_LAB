#define __LIBRARY__  /*使用系统调用*/
#include <unistd.h>         /* fork open 等 */
#include <sys/types.h>
#include <sys/wait.h>       /* wait */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX_NUM 521
#define CONSUMER_NUM 8
#define BUF_SIZE 10


int main()
{

    sem_t *mutex, *empty, *full;
    int fd; /* 文件描述符 */
    pid_t p;


    int i, j, data;
    int in = 0, out = 0;
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

    fd = open("buffer.dat",O_CREAT|O_RDWR|O_TRUNC,0666);

    lseek(fd, BUF_SIZE * sizeof(int), SEEK_SET);
    write(fd, (char *)&out, sizeof(int));       /* 多个消费者之间协调取数 */

    /* 生产者子进程 */
    if((p = fork()) == 0)
    {
        for(i = 0; i < MAX_NUM; i++)
        {
            sem_wait(empty);
            sem_wait(mutex);
            lseek(fd, in * sizeof(int), SEEK_SET);
            write(fd, (char *)&i, sizeof(int));
            in = (in + 1) % BUF_SIZE;
            sem_post(mutex);
            sem_post(full);
        }
        return 0;
    }
    else if(p < 0)
    {
        perror("create producer process failed!\n");
        return -1;
    }

    /* 消费者子进程 */
    for(i = 0; i < CONSUMER_NUM; i++)
    {
        if((p = fork()) == 0)
        {

            /*每个进程 最多取MAX_NUM次*/
            for(j = 0; j < MAX_NUM; j++)
            {
                sem_wait(full);
                sem_wait(mutex);

                lseek(fd, BUF_SIZE * sizeof(int), SEEK_SET);
                read(fd, (char *)&out, sizeof(int));

                lseek(fd, out * sizeof(int), SEEK_SET);
                read(fd, (char *)&data, sizeof(int));

                /*消费资源*/
                printf("%d:  %d\n",getpid(),data);
                fflush(stdout);

                out = (out + 1) % BUF_SIZE;
                lseek(fd, BUF_SIZE * sizeof(int), SEEK_SET);
                write(fd, (char *)&out, sizeof(int));

                sem_post(mutex);
                sem_post(empty);

                usleep(100);  /* 主动给其他进程机会 */
            }
            return 0;
        }
        else if(p < 0)
        {
            perror("create consumer process failed!\n");
            return -1;
        }
    }

    /* 等待子进程退出 */
    wait(NULL);
    sem_unlink("MUTEX");
    sem_unlink("EMPTY");
    sem_unlink("FULL");
    /* 关闭文件 */
    close(fd);
    return 0;
}