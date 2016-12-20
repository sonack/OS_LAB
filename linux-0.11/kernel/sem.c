#define __LIBRARY__
#include <unistd.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>

#define SEM_MAX_CNT 32

sem_t semaphores[SEM_MAX_CNT];

void init_queue(sem_queue *q)
{
    q->front = q->rear = 0;
}

int is_empty(sem_queue *q)
{
    return q->front == q->rear ? 1 : 0;
}

int is_full(sem_queue *q)
{
    return (q->rear + 1) % PCB_QUE_LEN == q->front ? 1 : 0;
}

struct task_struct * get_task(sem_queue *q)
{
    if(is_empty(q))
    {
        printk("PCB Queue is empty!\n");
        return NULL;
    }

    struct task_struct *task = q->wait_tasks[q->front];
    q->front = (q->front + 1) % PCB_QUE_LEN;
    return task;
}

int insert_task(struct task_struct *task, sem_queue *q)
{
    if(is_full(q))
    {
        printk("PCB Queue is full!\n");
        return -1;
    }
    q->wait_tasks[q->rear] = task;
    q->rear = (q->rear + 1) % PCB_QUE_LEN;
    return 1;
}

// 检查信号量是否已经打开
int sem_location(const char *name)
{
    int i;
    for(i = 0; i < SEM_MAX_CNT; i++)
    {
        if(strcmp(name, semaphores[i].name) == 0 && semaphores[i].occupied == 1)
        {
            return i;
        }
    }
    return -1;
}


sem_t * sys_sem_open(const char *name, unsigned int value)
{
    char tmp_name[32];
    int i = 0;
    while(tmp_name[i] = get_fs_byte(name + i), tmp_name[i++]);
    if(i >= 32)
    {
        printk("Semaphore name is too long! Don't exceed 32 chars!");
        return NULL;
    }
    /* 找到已存在的信号量 */
    if((i = sem_location(tmp_name)) != -1)
    {
        return &semaphores[i];
    }

    /* 创建新的信号量 */
    for(i = 0; i < SEM_MAX_CNT; i++)
    {
        if(!semaphores[i].occupied)
        {
            strcpy(semaphores[i].name,tmp_name);
            semaphores[i].occupied = 1;
            semaphores[i].value = value;
            init_queue(&(semaphores[i].wait_queue));
            return &semaphores[i];
        }
    }

    /* 信号量已达上限 */
    printk("There is no empty semaphore!");
    return NULL;
}

/* P Operation */
int sys_sem_wait(sem_t *sem)
{
    cli();   /* 关中断 */
    sem->value--;
    if(sem->value < 0)
    {
        current->state = TASK_UNINTERRUPTIBLE;  /* 阻塞线程 */
        insert_task(current,&(sem->wait_queue));
        schedule();
    }
    sti();   /* 开中断 */
    return 0;
}

/* V Operation */
int sys_sem_post(sem_t * sem)
{
    cli();
    sem->value++;
    struct task_struct *task;
    if(sem->value <= 0)
    {
        task = get_task(&(sem->wait_queue));
        if(task != NULL)
        {
            task->state = TASK_RUNNING;    // 使进程就绪，准备运行
        }
    }
    sti();
    return 0;
}

int sys_sem_unlink(const char *name)
{
    char tmp_name[32];
    int i = 0;
    while(tmp_name[i] = get_fs_byte(name + i), tmp_name[i++]);
    if(i >= 32)
    {
        printk("Semaphore name is too long! Don't exceed 32 chars!");
        return -1;
    }
    /* 该信号量存在 */
    if((i = sem_location(tmp_name)) != -1)
    {
        semaphores[i].value = 0;
        memset(semaphores[i].name,0,sizeof(semaphores[i].name));
        semaphores[i].occupied = 0;
        return 0;               /* 释放成功 */
    }
    return -1;  /* 信号量不存在 */
}