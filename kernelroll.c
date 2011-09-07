/*
 * kernelroll - linux kernel module for advanced rickrolling
 * Copyright (C) 2011 Franz Pletz <fpletz@fnordicwalking.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <asm/unistd.h> 
#include <linux/syscalls.h>
#include <asm/amd_nb.h>
#include <linux/highuid.h>

#define GPF_DISABLE write_cr0(read_cr0() & (~ 0x10000))
#define GPF_ENABLE write_cr0(read_cr0() | 0x10000)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Franz Pletz");
MODULE_DESCRIPTION("for teh lulz!");

char *rollfile;
void **sys_call_table = (void **)0xffffffff81400300; /* TODO: change */

module_param(rollfile, charp, 0000);
MODULE_PARM_DESC(rollfile, "music trolling file");

module_param(sys_call_table, ulong, 0000);
MODULE_PARM_DESC(sys_call_table, "address of the system call table");

/* currently not working try for finding the sys_call_table ourselves */
unsigned long **find_sys_call_table(void) 
{
    unsigned long **sctable;
    unsigned long ptr;

    sctable = NULL;
    for (ptr = (unsigned long)&amd_nb_misc_ids;
            ptr < (unsigned long)&overflowgid; 
            ptr += sizeof(void *))
    {
        unsigned long *p;
        p = (unsigned long *)ptr;
        if(p[__NR_close] == (unsigned long) sys_close)
        {
            sctable = (unsigned long **)p;
            return &sctable[0];
        }
    }
    return NULL;
}


asmlinkage int (*o_open)(const char *path, int oflag, mode_t mode); 
asmlinkage int my_open(const char *path, int oflag, mode_t mode) 
{
    int len = strlen(rollfile) + 1;
    char* p;
    int r;

    p = (char *)(path + strlen(path) - 4);

    if(rollfile != NULL && !strcmp(p, ".mp3")) {
        void *buf = kmalloc(len, GFP_KERNEL);
        memcpy(buf, path, len);
        printk(KERN_INFO "patching %s with %s\n", path, rollfile);
        memcpy((void *)path, rollfile, len);
        r = o_open(path, oflag, mode);
        memcpy((void *)path, buf, len);
        kfree(buf);
    } else {
        r = o_open(path, oflag, mode);
    }


    return r;
} 


void set_addr_rw(unsigned long addr) {

    unsigned int level;
    pte_t *pte = lookup_address(addr, &level);

    if(pte->pte &~ _PAGE_RW) pte->pte |= _PAGE_RW;

}

void set_addr_ro(unsigned long addr) {

    unsigned int level;
    pte_t *pte = lookup_address(addr, &level);

    pte->pte = pte->pte &~_PAGE_RW;

}

static int __init init_rickroll(void) 
{
    //sys_call_table = find_sys_call_table();
    if(sys_call_table == NULL)
    {
        printk(KERN_ERR "Cannot find the system call address\n"); 
        return -1;  /* do not load */
    } else {
        printk(KERN_INFO "System call table found @ %lx\n", (unsigned long)sys_call_table);
    }

    set_addr_rw((unsigned long)sys_call_table);
    GPF_DISABLE;

    o_open = (int(*)(const char *, int, mode_t))(sys_call_table[__NR_open]); 
    sys_call_table[__NR_open] = (void *) my_open; 

    return 0; 
} 

static void __exit exit_rickroll(void) 
{ 
    sys_call_table[__NR_open] = (void *) o_open; 

    set_addr_ro((unsigned long)sys_call_table);
    GPF_ENABLE;
} 

module_init(init_rickroll); 
module_exit(exit_rickroll); 
