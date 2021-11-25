#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 1024
int main(int argc, char **argv)
{
	int rfd, wfd;
	char buf[MAXLINE];

	rfd = open("/tmp/myfifo_w", O_RDWR);
	if (rfd < 0)
	{
		perror("read open error\n");
		return 1;
	}
	wfd = open("/tmp/myfifo_r", O_RDWR); // open named pipe myfifo_r as write buffer
	if (wfd < 0)
	{
		perror("write open error\n");
		return 1;
	}
	while (1)
	{
		printf("> ");
		fflush(stdout);
		memset(buf, 0x00, MAXLINE);
		if (read(0, buf, MAXLINE) < 0) // input from stdin
		{
			printf("error\n");
			return 1;
		}
		if (strncmp(buf, "quit\n", 5) == 0) // if 'quit' -> exit
			break;
		write(wfd, buf, strlen(buf)); // write -> to
		read(rfd, buf, MAXLINE);	  // read
		printf("Server -> %s", buf);
	}
	close(wfd);
	close(rfd);
	return 0;
}
