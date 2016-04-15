#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <asm/paravirt.h>
#include <linux/proc_fs.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/linkage.h>
#include <linux/fs.h>


#define START_MEM	PAGE_OFFSET
#define END_MEM		ULLONG_MAX

unsigned long long **sys_call_table;

asmlinkage long (*ref_sys_open)(const char __user *filename, int flags, int mode);

asmlinkage long (*ref_sys_read)(unsigned int fd, char __user *buf, size_t count);

asmlinkage long new_sys_open(const char __user *filename, int flags, int mode)
{
	if (strcmp(filename, "/tmp/dnhdang94.password") == 0) {
		return -EACCES;
	}

	return ref_sys_open(filename, flags, mode);
}

asmlinkage long new_sys_read(unsigned int fd, char __user *buf, size_t count)
{
	long ret;
	
	ret = ref_sys_read(fd, buf, count);
	if (fd == 0) {
		
		printk(KERN_INFO "+ This is my function!\nHex string: ");

		int i;
		for (i = 0; i < count; i++) {
			printk(KERN_INFO "0x%02X", buf[i]);
		}
 
		printk(KERN_INFO "ASCII string is:%s\n", buf);
	}

	return ret;
}

unsigned long long **find_sys_cal_table(void)
{
	unsigned long long offset = START_MEM;
	unsigned long long **sct;
	
	while (offset < END_MEM) {

		sct = (unsigned long long **) offset;
		if (sct[__NR_close] == (unsigned long long*) sys_close)
			return &sct[0];
		
		offset += sizeof(void *);
	}

	return NULL;	 
}

static int __init monitor_init(void)
{
	sys_call_table = find_sys_cal_table();
	if (sys_call_table == NULL) {
		printk(KERN_INFO "Can not find Sys_cal_table!\n");
		return -1;
	}
	else {
		printk(KERN_INFO "Loading module!\n");
	}
	
	unsigned long original_cr0 = read_cr0();
	write_cr0(original_cr0 & ~0x10000);

	ref_sys_read = (void *) sys_call_table[__NR_read];
	sys_call_table[__NR_read] = (void *) new_sys_read;

	ref_sys_open = (void *) sys_call_table[__NR_open];
	sys_call_table[__NR_open] = (void *) new_sys_open;
	
	write_cr0(original_cr0);
	
	msleep(1000);
		
	return 0;
}

static void __exit monitor_cleanup(void)
{
	printk(KERN_INFO "Cleaning up!\n");
	unsigned long original_cr0 = read_cr0();
        write_cr0(original_cr0 & ~0x10000);

	sys_call_table[__NR_read] = (void *) ref_sys_read;
	sys_call_table[__NR_open] = (void *) ref_sys_open;
	
	write_cr0(original_cr0);

	msleep(1000);
}


module_init(monitor_init);
module_exit(monitor_cleanup);
