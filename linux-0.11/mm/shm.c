/* 实现系统调用 */
/* 
    int shmget(int key);
    void *shmat(int shmid);
*/

#include <asm/segment.h>
#include <linux/kernel.h>
#include <unistd.h>
#include <linux/sched.h>
#include <string.h>
#include <errno.h>

#define SHM_MAX_CNT 40

struct struct_shm_tables
{
    int occupied;
    int key;
    unsigned long addr;
}shm_tables[SHM_MAX_CNT];

/* 下标即为OS内部的shmid编号 */

int find_shm_location(int key)
{
    int i = 0;
    for(i = 0; i < SHM_MAX_CNT; i++)
    {
        if(shm_tables[i].key == key && shm_tables[i].occupied  == 1)
        {
            return i;
        }
    }
    return -1;
}

int sys_shmget(int key)
{
    int i,
        shmid = find_shm_location(key);
    if(shmid != -1)         /* 返回已经存在的共享内存对象 */
        return shmid;

    /* 新建共享内存对象 */
    for(i = 0; i < SHM_MAX_CNT; i++)
    {
        if(shm_tables[i].occupied == 0)
        {
            shm_tables[i].key = key;
            shm_tables[i].occupied = 1;
            shm_tables[i].addr = get_free_page();   /* 获得空闲物理页面地址 */
            return i;       /* 返回shmid */
        }
    }
    printk("There is no share memory available!\n");
    return -ENOMEM;
}

void *sys_shmat(int shmid)
{
    if(shm_tables[shmid].occupied == 0)
    {
        printk("The share memory does not exist!\n");
        return -EINVAL;
    }
    put_page(shm_tables[shmid].addr, current->brk + current->start_code); /* current->brk 堆的当前最后地址 */
    return (void *)current->brk;  /* 返回虚拟地址 */
}






