#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main()
{
	const int SIZE = 4096;
	const char *name = "OS";
	const char* message0= "OSisFun";
	const char* message1= "freeeee";

	const char* fifo_w = "/tmp/wfifo"; // to communicate index written into
	const char* fifo_r = "/tmp/rfifo"; // to communicate index read from
	const char* fifo_c = "/tmp/confirm"; // to syncronise at the end

	mkfifo(fifo_w, 0666);
	mkfifo(fifo_r, 0666);
	mkfifo(fifo_c, 0666);

	int fifo_write = open(fifo_w, O_WRONLY);
	int fifo_read = open(fifo_r, O_RDONLY);
	int confirm = open(fifo_c, O_RDONLY);

	int shm_fd;
	void *ptr;

	/* create the shared memory segment */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	/* configure the size of the shared memory segment */
	ftruncate(shm_fd,SIZE);

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}

	for(int i = 0 ; i < SIZE/8 ; ++i){
		memcpy(ptr+i*8, message1, strlen(message1)+1);
	}

	printf("Producer wrote freeeee 512 times\n");

	for(int i = 0 ; i < SIZE/8 ; ++i){
		memcpy(ptr+i*8, message0, strlen(message0)+1);
		write(fifo_write, &i, sizeof(int));
		printf("Producer written at index %d\n", i);
	}

	printf("Initial OS written\n");

	for(int i = 0 ; i < 1000-SIZE/8 ; ++i){
		int j;
		read(fifo_read, &j, sizeof(int));
		memcpy(ptr+j*8, message0, strlen(message0)+1);
		write(fifo_write, &j, sizeof(int));
		printf("Producer written at index %d\n", j);
	}

	char c;
	read(confirm, &c, 1);

	close(fifo_write);
	close(fifo_read);
	close(confirm);

	return 0;
}
