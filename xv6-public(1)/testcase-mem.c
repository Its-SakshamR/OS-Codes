#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(void)
{
	int free = getNumFreePages();
	printf(1, "Free pages: %d\n", free);

	printf(1, "Allocating 4 pages using sbrk\n");
	char* addr = (char*)sbrk(4 * 4096);
	for(int i=0;i<4*4096;i++){
		addr[i] = 'a';
	}
	free = getNumFreePages();
	printf(1, "Free pages: %d\n", free);
	if(fork() == 0){
		printf(1, "Forked a process\n");
		free = getNumFreePages();
		printf(1, "Free pages: %d\n", free);
		addr[4097] = 'b';
		printf(1,"accessed 1 page in child\n");
		free = getNumFreePages();
		printf(1, "Free pages: %d\n", free);
	}else{
		wait();
		printf(1,"Child exited\n");
		printf(1,"Value at addr[4097] = %c\n", addr[4097]);
		free = getNumFreePages();
		printf(1, "Free pages: %d\n", free);
	}
	exit();
}
