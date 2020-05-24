#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>

#define BUFFER_LENGTH 600000

int main(){
    char user_buffer[BUFFER_LENGTH];
	int fd_read = open("/proc/pf_probe_I1", O_RDONLY);      
	if (fd_read < 0){
		return errno;
	}
	printf("Reading from user buffer ... \n");
	ssize_t bytes_read = read(fd_read, user_buffer, BUFFER_LENGTH);
	close(fd_read);
	printf("Writing to %s ...\n", "plot.txt");
    int fd_write = open("plot.txt", O_RDWR | O_CREAT);
	write(fd_write, &user_buffer, sizeof(user_buffer));
    close(fd_write);
	printf("%s populated\n", "plot.txt");
	return 0;
}