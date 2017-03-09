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


void coalesce(void* ptr);
sf_free_header* bestFit(sf_free_header* freelist_head,size_t requestedSize);
void init(sf_free_header* freelist_head,size_t alloc);
/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;
int pages = 0;
size_t currAllocated = 0;
size_t totalAllocated = 0;

void *sf_malloc(size_t size) {
	//size_t newSize = 0;
	//sf_free_header* ptrToFreeBlock = NULL;
	if(size == 0)													//if size = 0, return null and invalid argument
	{
		errno = EINVAL;
		return NULL;
	}

	//newSize = SF_HEADER_SIZE + SF_FOOTER_SIZE + size;					//add footer and header size
	//debug("Footer + Header + size = %zu\n",newSize);
	//if(size <= QWSIZE)
	//	newSize = 2*QWSIZE;
	//else
	debug("RequestedSize = %zu\n",size);
	size_t blockSize = 16*((size + 16 + (16 -1)) / 16);						//make block size 16 addressable
	debug("BlockSize = %zu\n",blockSize);
	size_t paddingSize = blockSize - (16 + size);								//16 from header and footer
	debug("PaddingSize = %zu\n",paddingSize);

		currAllocated += blockSize;
		totalAllocated += blockSize;
		int addPages = 0;
		if(currAllocated >= 4096)
		{
			int pageAllocate = currAllocated/4096;
			size_t leftover = currAllocated % 4096;
			currAllocated = 0;
			currAllocated += leftover;
			addPages = pageAllocate+1;

		}

		if(freelist_head == NULL)               										//intialize head of freelist and create 1 page of mem
		{
			addPages = blockSize/4096 + 1;
			size_t alloc = addPages * 4096;
        	freelist_head = (sf_free_header*)sf_sbrk(alloc);		//more than 1 page, allocate necessary amount
        	freelist_head = freelist_head + 8;											//move brk to address divisble by 8 for header and payload divisible by 16
        	debug("%p\n",freelist_head);
        	init(freelist_head,alloc);

        }
       /* else if(freelist_head->next != NULL)
        {
        	if((ptrToFreeBlock = bestFit(freelist_head,blockSize)) == NULL)	//find bestfit position
        	{
        		sf_free_header* tempHeader = sf_sbrk(4096 * addPages);
        		ptrToFreeBlock = bestFit(tempHeader,blockSize);
        	}
        }
        else if(freelist_head->next == NULL && freelist_head->prev == NULL)	//no fragements; memory just allocated.
        {
        	freelist_head = sf_sbrk(4096 * addPages);
        	ptrToFreeBlock = freelist_head;
        }*/

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

        if(freelist_head->next != NULL)
        	freelist_head = freelist_head->next;
        else
        	freelist_head = (sf_free_header*)((char*)ftptr + QWSIZE);	//move header address to next avaliable free block

        size_t freeBlockSize =  pages*4096 - totalAllocated;
        sf_footer* ftFreeList = (sf_footer*)(((char*)freelist_head) + freeBlockSize -QWSIZE);
        freelist_head->header.block_size = freeBlockSize >> 4;
        ftFreeList->block_size = freeBlockSize >>4 ;

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

	if(size != fptr->block_size || hptr->alloc == 0)	//if header alloc does not match with footer address is invalid
	{
		errno = EINVAL;
		return;
	}

	(hptr->alloc) = 0;									//set header alloc bit to 0;
	(fptr->alloc) = 0;									//set footer alloc bit to 0;
	(hptr->requested_size) = 0;

	if((void*)hptr < (void*)freelist_head)				//if current header address is less than freelist header address
	{
		sf_free_header* temp = freelist_head;
		freelist_head->prev = (sf_free_header*)hptr;
		freelist_head = (sf_free_header*)hptr;
		freelist_head->next = temp;
	}
	else
	{
		sf_free_header* address = freelist_head;
		while(address != NULL)
		{
			if(((void*)hptr) > ((void*)address) && ((void*)hptr) < (void*)(address->next))
			{
				sf_free_header* temp = (sf_free_header*)hptr;
				temp->next = address->next;
				address->next->prev = temp;
				address->next = temp;
				temp->prev = address;
			}
			address = address->next;
		}
	}

	coalesce(ptr);
	return;
}

int sf_info(info* ptr) {
	return -1;
}

void coalesce(void* ptr)
{
	return;
}

sf_free_header* bestFit(sf_free_header* freelist_head,size_t requestedSize)
{
	sf_free_header* currptr = freelist_head;
	sf_free_header* smallestSize = NULL;
	while(currptr != NULL)
	{
		if((currptr->header.block_size)== requestedSize)
			return currptr;
		if(((currptr->header.block_size) > requestedSize) && ((currptr->header.block_size) < (smallestSize->header.block_size)))
			smallestSize = currptr;

		currptr = currptr->next;
	}

	return smallestSize;
}

void init(sf_free_header* freelist_head, size_t alloc)
{
	sf_header *head = (sf_header*)freelist_head;
	sf_footer *footer = (sf_footer*)(((char*)freelist_head) + alloc - QWSIZE);
	head->block_size = alloc;
	footer->block_size = alloc;

	return;
}
