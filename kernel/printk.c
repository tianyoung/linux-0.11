/*
 *  linux/kernel/printk.c
 *
 *  (C) 1991  Linus Torvalds
 */

/*
 * When in kernel-mode, we cannot use printf, as fs is liable to
 * point to 'interesting' things. Make a printf with fs-saving, and
 * all is well.
 */
#include <stdarg.h>
#include <stddef.h>

#include <linux/kernel.h>
#include <linux/sched.h>

static char buf[1024];

extern int vsprintf(char * buf, const char * fmt, va_list args);

int printk(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	__asm__("push %%fs\n\t"
		"push %%ds\n\t"
		"pop %%fs\n\t"
		"pushl %0\n\t"
		"pushl $buf\n\t"
		"pushl $0\n\t"
		"call tty_write\n\t"
		"addl $8,%%esp\n\t"
		"popl %0\n\t"
		"pop %%fs"
		::"r" (i):"ax","cx","dx");
	return i;
}


static char logbuf[1024];

// reference printk and sys_write
int fprintk(unsigned int fd, const char *fmt, ...)
{
    va_list args;
    int count;
    struct file * file;
    struct m_inode * inode;

    va_start(args, fmt);
    count=vsprintf(logbuf, fmt, args);
    va_end(args);




    if (fd < 3)
    {
 	__asm__("push %%fs\n\t"
		"push %%ds\n\t"
		"pop %%fs\n\t"
		"pushl %0\n\t"
		"pushl $logbuf\n\t"
		"pushl $0\n\t"
		"call tty_write\n\t"
		"addl $8,%%esp\n\t"
		"popl %0\n\t"
		"pop %%fs"
		::"r" (count):"ax","cx","dx");   

       // __asm__("push %%fs\n\t"
       //     "push %%ds\n\t"
       //     "pop %%fs\n\t"
       //     "pushl %0\n\t"
       //     "pushl $logbuf\n\t"
       //     "pushl %1\n\t"
       //     "call sys_write\n\t"
       //     "addl $8,%%esp\n\t"
       //     "popl %0\n\t"
       //     "pop %%fs"
       //     ::"r" (count),"r" (fd):"ax","cx","dx");
    }
    else
    {
        if (!(file=task[0]->filp[fd]))
            return 0;
        inode=file->f_inode;

        __asm__("push %%fs\n\t"
            "push %%ds\n\t"
            "pop %%fs\n\t"
            "pushl %0\n\t"
            "pushl $logbuf\n\t"
            "pushl %1\n\t"
            "pushl %2\n\t"
            "call file_write\n\t"
            "addl $12,%%esp\n\t"
            "popl %0\n\t"
            "pop %%fs"
            ::"r" (count),"r" (file),"r" (inode):"ax","cx","dx");
    }
    return count;
}

//void printplog(long pid, char X, long time)
//{
//	static char first = 1;
//	
//	int test = 99;
//	const char *str = "test string";
//
//	if(first) {
//		//printk("hello world: %s\n", str);
//		//printk("%ld	%c	%ld\n", 99, 'R', 199);
//		//fprintk(1, "%ld\t%c\t%ld\n", 99, 'R', 199);
//		//fprintk(1, "%ld\t%c\t%ld\n", 9527, 'X', 2345);
//		first = 0;
//	}
//}
