#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>
#include<stdio.h>

int main ()
{
	int m_fd, c_fd;
	char *m_src = "/sys/bus/platform/drivers/pru_speak/4a300000.prurproc/pru_speak_binfile";
	char *c_src = "/sys/bus/platform/drivers/pru_speak/4a300000.prurproc/pru_speak_control";
	 
	m_fd = open(m_src, O_RDWR);
	c_fd = open(c_src, O_RDWR);
	 
	 /* mmap the input file */
	int *src = (int *)mmap (NULL, 4096, PROT_READ, MAP_PRIVATE, m_fd, 0);
	if((int)src < 0 )
	{
		printf("error");
		perror("mmap failed");
	}
	printf("MMAP value : %d\n", src);
	
	write(c_fd, '1', 1);
	printf("MMAP value : %d\n", *src);
	
	close(m_fd);
	close(c_fd);
	return 0;
}


