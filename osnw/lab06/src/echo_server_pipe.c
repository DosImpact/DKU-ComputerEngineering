#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 1024

// Named PIPE Example echoServer)
int main(int argc, char **argv)
{
	int rfd, wfd;
	char buf[MAXLINE];

	mkfifo("/tmp/myfifo_r", S_IRUSR | S_IWUSR); // make read FIFO ( pipe )
	mkfifo("/tmp/myfifo_w", S_IRUSR | S_IWUSR); // make write FIFO ( pipe )
	if ((rfd = open("/tmp/myfifo_r", O_RDWR)) == -1)
	{
		perror("rfd error");
		return 0;
	}
	if ((wfd = open("/tmp/myfifo_w", O_RDWR)) == -1)
	{
		perror("wfd error");
		return 0;
	}
	while (1)
	{
		memset(buf, 0x00, MAXLINE);
		if (read(rfd, buf, MAXLINE) < 0) // try read pipe
		{
			perror("Read Error");
			return 1;
		}
		printf("Read : %s", buf);
		write(wfd, buf, MAXLINE); // ?
		lseek(wfd, 0, SEEK_SET);  // ?
	}
}
