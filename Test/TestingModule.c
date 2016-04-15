#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
	int fd;
	int err;
	char *buf = (char *) malloc(10);

	//fd = open("test.txt", O_RDONLY);
	//if (fd < 0) {
	//	printf("Can not open file!\n");
	//	free(buf);
	//	return 1;
	//}

	err = read(0, buf, 10);
	printf("Return: %d\n", err);

	free(buf);
	close(fd);

	return 0;
}
