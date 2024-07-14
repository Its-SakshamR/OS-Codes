#include "simplefs-ops.h"
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

int min(int a, int b){
	if(a < b) return a;
	return b;
}

int simplefs_create(char *filename){
    /*
	    Create file with name `filename` from disk
	*/
	// int i = 0;
	struct inode_t inodeptr;

	for(int i = 0 ; i < NUM_INODES ; i++){
		simplefs_readInode(i, &inodeptr);
		if(inodeptr.status == INODE_IN_USE && strcmp(inodeptr.name, filename) == 0){
			return -1;
		}
	}

	int inode_num = simplefs_allocInode();
	if(inode_num == -1) return -1;
	// simplefs_readInode(inode_num, &inodeptr);
	inodeptr.status = INODE_IN_USE;
	inodeptr.file_size = 0;
	strcpy(inodeptr.name, filename);

	for(int i = 0 ; i < MAX_FILE_SIZE ; i++){
		inodeptr.direct_blocks[i] = -1;
	}
	simplefs_writeInode(inode_num, &inodeptr);
	return inode_num;
}


void simplefs_delete(char *filename){
    /*
	    delete file with name `filename` from disk
	*/
	struct inode_t inodeptr;

	for(int i = 0 ; i < NUM_INODES ; i++){
		simplefs_readInode(i, &inodeptr);
		if(inodeptr.status == INODE_IN_USE && strcmp(inodeptr.name, filename) == 0){
			for(int i = 0 ; i < MAX_FILE_SIZE ; i++){
				if(inodeptr.direct_blocks[i] != -1){
					simplefs_freeDataBlock(inodeptr.direct_blocks[i]);
				}
			}
			strcpy(inodeptr.name,"");
			simplefs_writeInode(i, &inodeptr);
			simplefs_freeInode(i);
			break;
		}
	}
}

int simplefs_open(char *filename){
    /*
	    open file with name `filename`
	*/
    struct inode_t inodeptr;
	int inode_num = -1;

	for(int i = 0 ; i < NUM_INODES ; i++){
		simplefs_readInode(i, &inodeptr);
		if(inodeptr.status == INODE_IN_USE && strcmp(inodeptr.name, filename) == 0){
			inode_num = i;
			break;
		}
	}

	if(inode_num == -1) return -1;

	for(int i = 0 ; i < MAX_OPEN_FILES ; i++){
		if(file_handle_array[i].inode_number == -1){
			file_handle_array[i].inode_number = inode_num;
			file_handle_array[i].offset = 0;
			return i;
		}
	}

	return -1;
}

void simplefs_close(int file_handle){
    /*
	    close file pointed by `file_handle`
	*/
	file_handle_array[file_handle].inode_number = -1;
	file_handle_array[file_handle].offset = 0;
}

int simplefs_read(int file_handle, char *buf, int nbytes){
    /*
	    read `nbytes` of data into `buf` from file pointed by `file_handle` starting at current offset
	*/
    int new_offset = file_handle_array[file_handle].offset;
	int inode_num = file_handle_array[file_handle].inode_number;

	struct inode_t inodeptr;

	simplefs_readInode(inode_num, &inodeptr);
	if(nbytes < 0 || new_offset + nbytes > inodeptr.file_size) return -1;

	char temp[MAX_FILE_SIZE * BLOCKSIZE];

	for(int i = 0 ; i < MAX_FILE_SIZE ; i++){
		if(inodeptr.direct_blocks[i] != -1){
			simplefs_readDataBlock(inodeptr.direct_blocks[i], temp+i*BLOCKSIZE);
		}
	}

	strncpy(buf, temp + new_offset, nbytes);
	return 0;
}


int simplefs_write(int file_handle, char *buf, int nbytes){
    /*
	    write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset
	*/
    int new_offset = file_handle_array[file_handle].offset;
	int inode_num = file_handle_array[file_handle].inode_number;

	struct inode_t inodeptr;
	simplefs_readInode(inode_num, &inodeptr);
	if(inodeptr.status == INODE_FREE) return -1;
	if(nbytes < 0 || new_offset + nbytes > MAX_FILE_SIZE * BLOCKSIZE) return -1;


	int offset_at_curr_block = new_offset%BLOCKSIZE;

	int mem_copied = 0;

	int num_curr_block = 0;

	if(offset_at_curr_block == 0){
		num_curr_block = new_offset / BLOCKSIZE ;
	}

	else num_curr_block = 	new_offset / BLOCKSIZE + 1;

	int num_block_req;

	if((new_offset+nbytes)%BLOCKSIZE == 0){
		num_block_req = (new_offset + nbytes)/BLOCKSIZE;
	}

	else num_block_req = (new_offset + nbytes)/BLOCKSIZE + 1;

	for(int i = num_curr_block ; i < num_block_req ; i++){
		if(inodeptr.direct_blocks[i] == -1){
			int new_datablock = simplefs_allocDataBlock();
			if (new_datablock == -1){
				for(int j = num_curr_block ; j < num_block_req ; j++){
					if(inodeptr.direct_blocks[i] != -1) simplefs_freeDataBlock(inodeptr.direct_blocks[i]);
					else break;
				}
				return -1;
			}
			inodeptr.direct_blocks[i] = new_datablock;
		}
	}

	int curr_block_index = num_curr_block;

	if(offset_at_curr_block != 0){
		curr_block_index = num_curr_block - 1;
		int curr_block_num = inodeptr.direct_blocks[curr_block_index];
		int bytes_to_add = min(BLOCKSIZE - offset_at_curr_block, nbytes);
		char temp[BLOCKSIZE];
		simplefs_readDataBlock(curr_block_num, temp);

		strncpy(temp + offset_at_curr_block, buf, bytes_to_add);
		for(int i = offset_at_curr_block + bytes_to_add ; i < BLOCKSIZE ; i++){
			temp[i] = '\0';
		}

		simplefs_writeDataBlock(curr_block_num, temp);
		mem_copied += bytes_to_add;
		curr_block_index++;
	}

	while(mem_copied < nbytes){
		int curr_block_num = inodeptr.direct_blocks[curr_block_index];

		char temp[BLOCKSIZE];
		int bytes_to_add = min(BLOCKSIZE, nbytes - mem_copied);
		strncpy(temp, buf + mem_copied, bytes_to_add);

		for(int i = bytes_to_add ; i < BLOCKSIZE ; i++){
			temp[i] = '\0';
		}

		simplefs_writeDataBlock(curr_block_num, temp);
		mem_copied += bytes_to_add;
		curr_block_index++;
	}

	inodeptr.file_size += mem_copied;
	simplefs_writeInode(inode_num, &inodeptr);

	return 0;
}


int simplefs_seek(int file_handle, int nseek){
    /*
	   increase `file_handle` offset by `nseek`
	*/
	int new_offset = file_handle_array[file_handle].offset;

	if(new_offset + nseek < 0 && new_offset + nseek > MAX_FILE_SIZE * BLOCKSIZE) return -1;

	file_handle_array[file_handle].offset = new_offset + nseek;

	return 0;
}