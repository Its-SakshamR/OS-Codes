#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int main()
{
	const char *name = "OS";
	const int SIZE = 4096;

	const char* fifo_w = "/tmp/wfifo"; // to communicate index written into
	const char* fifo_r = "/tmp/rfifo"; // to communicate index read from
	const char* fifo_c = "/tmp/confirm"; // to syncronise at the end

	mkfifo(fifo_w, 0666);
	mkfifo(fifo_r, 0666);
	mkfifo(fifo_c, 0666);

	int fifo_write = open(fifo_w, O_RDONLY);
	int fifo_read = open(fifo_r, O_WRONLY);
	int confirm = open(fifo_c, O_WRONLY);

	int shm_fd;
	void *ptr;

	/* open the shared memory segment */
	shm_fd = shm_open(name, O_RDWR, 0666);
	if (shm_fd == -1) {
		printf("shared memory failed\n");
		exit(-1);
	}

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0,SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}

	/* now read from the shared memory region */
	char* str = (char*)malloc(8);

	/* remove the shared memory segment */
	for(int i = 0 ; i < 1000 ; ++i){
		int j;
		read(fifo_write, &j, sizeof(int));
		memcpy(str, ptr+8*j, 8);
		write(fifo_read, &j, sizeof(int));
		printf("Consumer received %s at inded %d\n", str, j);
		sleep(1);
	}

	write(confirm, "c", 1);

	close(fifo_write);
	close(fifo_read);
	close(confirm);

	return 0;
}
