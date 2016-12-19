#include <string.h>
#include <errno.h>  // include/errno.h
#include <asm/segment.h>    // get_fs_byte put_fs_byte
char uname[24]; // + '\0'
int sys_iam(const char * name)
{
    int i = 0;
    while( uname[i] = get_fs_byte(name + i) ) i++;
    // printk("sys_iam was called!\n");
    if(i > 23)
        return -EINVAL;
    return i+1;  // '\0' 
}

int sys_whoami(char* name, unsigned int size)
{
    int i = 0;
    while( put_fs_byte(uname[i], name + i), uname[i] ) i++;
    // printk("sys_whoami was called!\n");
    if(i >= size) // = '\0'
        return -EINVAL;
    return i+1;
}

