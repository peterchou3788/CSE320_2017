/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <errno.h>
#include "sfmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <debug.h>

#define QWSIZE 8		//Quadword = 64 bits = 8 bytes

#define HDADD(p) ((char*)p - QWSIZE)		//get address of Header
#define FTADD(p) ((char*)p + )


void coalesce(void* ptr);
/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;
int pages = 0;

void *sf_malloc(size_t size) {
	size_t newSize = 0;
	if(size == 0)													//if size = 0, return null and invalid argument
	{
		errno = EINVAL;
		return NULL;
	}

	newSize = SF_HEADER_SIZE + SF_FOOTER_SIZE + size;					//add footer and header size
	//debug("Footer + Header + size = %zu\n",newSize);
	//if(size <= QWSIZE)
	//	newSize = 2*QWSIZE;
	//else
	debug("RequestedSize = %zu\n",size);
	size_t blockSize = 16*((size + 16 + (16 -1)) / 16);						//make block size 16 addressable
	debug("BlockSize = %zu\n",blockSize);
	//size_t blockSize = newSize + (newSize % 16);
	size_t paddingSize = blockSize - (16 + size);								//16 from header and footer
	debug("PaddingSize = %zu\n",paddingSize);


		int addPages = (newSize/4096) + 1;
		if(freelist_head == NULL)               						//intialize head of freelist and create 1 page of mem
		{
        	freelist_head = (sf_free_header*)sf_sbrk(addPages*4096);		//more than 1 page, allocate necessary amount
        	freelist_head = freelist_head + 8;								//move brk to address divisble by 8 for header and payload divisible by 16
        	debug("%p\n",freelist_head);
        }
        else
        {
        	sf_sbrk(addPages * 4096);
        }
        pages += addPages;

        if(pages > 4)
        	return NULL;


        sf_header* hdptr = (sf_header*)freelist_head;							//address of header
        void* memptr = ((char*)freelist_head) + QWSIZE;							//address of payload
        sf_footer* ftptr = (sf_footer*)((char*)hdptr + blockSize - QWSIZE); 	//address of footer

        debug("%p\n",hdptr);
        debug("%p\n",memptr);
        debug("%p\n",ftptr);

        hdptr->alloc = 1;
        ftptr->alloc = 1;
        hdptr->block_size = blockSize >> 4;
        ftptr->block_size = blockSize >> 4;
        hdptr->requested_size = size;
        hdptr->padding_size = paddingSize;


	return memptr;
}

void *sf_realloc(void *ptr, size_t size) {
	return NULL;
}

void sf_free(void* ptr) {
	//char* header = ((char*)ptr) - QWSIZE;
	//char* footer = NULL;
	sf_header* hptr = (sf_header*)ptr - QWSIZE;
	size_t size = (hptr->block_size)/4;					//make except last 34 bits
	sf_footer* fptr = (sf_footer*)ptr + size - QWSIZE;

	if(size != fptr->block_size)						//if header alloc does not match with footer address is invalid
	{
		errno = EINVAL;
		return;
	}

	(hptr->alloc) = 0;									//set header alloc bit to 0;
	(fptr->alloc) = 0;									//set footer alloc bit to 0;


	coalesce(ptr);
	return;
}

int sf_info(info* ptr) {
	return -1;
}

void coalesce(void* ptr)
{

}

