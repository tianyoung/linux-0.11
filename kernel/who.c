#include <linux/kernel.h>
#include <asm/segment.h>
#include <errno.h>


char name_buf[24];

int sys_iam(const char *name)
{
	int i = 0;
	int len;
	while(get_fs_byte(&name[i]) != '\0') {
		i++;
	}

	if(i > 23) return -EINVAL;
	len = i;

	while(i >= 0) {
		name_buf[i] = get_fs_byte(&name[i]);
		i--;
	}
	return len;
}

int sys_whoami(char *name, unsigned int size)
{
	int i  = 0;
	if(!name) return -1;
	verify_area(name, size);
	while(name_buf[i] != '\0') {
		if(i + 1 == size) {
			return -EINVAL;
		}
		put_fs_byte(name_buf[i], &name[i]);
		i++;
	}
	put_fs_byte('\0', &name[i]);
	return i;
}
