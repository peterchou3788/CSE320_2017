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


void* coalesce(void* ptr1,void* ptr2);
sf_free_header* bestFit(sf_free_header* freelist_head,size_t requestedSize);
void init(sf_free_header* freelist_head,size_t alloc);
/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;
int pages = 0;
int addPages = 0;
size_t currAllocated = 0;
size_t totalAllocated = 0;
size_t blockSize = 0;
size_t paddingSize = 0;
size_t splinterSize = 0;

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
	if(freelist_head == NULL)               										//intialize head of freelist and create 1 page of mem
	{
		debug("RequestedSize = %zu\n",size);
		blockSize = 16*((size + 16 + (16 -1)) / 16);						//make block size 16 addressable
		debug("BlockSize = %zu\n",blockSize);
		paddingSize = blockSize - (16 + size);								//16 from header and footer
		debug("PaddingSize = %zu\n",paddingSize);

		currAllocated += blockSize;
		totalAllocated += blockSize;

		if(currAllocated >= 4096)
		{
			int pageAllocate = currAllocated/4096;
			size_t leftover = currAllocated % 4096;
			currAllocated = 0;
			currAllocated += leftover;
			addPages = pageAllocate+1;

		}

		addPages = blockSize/4096 + 1;
		size_t alloc = addPages * 4096;
        freelist_head = (sf_free_header*)sf_sbrk(alloc);		//more than 1 page, allocate necessary amount
        freelist_head = freelist_head + 8;											//move brk to address divisble by 8 for header and payload divisible by 16
        debug("%p\n",freelist_head);
        init(freelist_head,alloc);

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

    else																//do best fit otherwise
    {
    	blockSize = 16*((size + 16 + (16 -1)) / 16);


    	sf_free_header* freeBlockPtr = (sf_free_header*)bestFit(freelist_head,blockSize);	//search freelist for best fit address
    	if(pages >4)
    		return NULL;
    	size_t freeBlockSize = freeBlockPtr->header.block_size;									//get block size of free block

    	debug("RequestedSize = %zu\n",size);
		blockSize = freeBlockSize;						//make block size 16 addressable
		debug("BlockSize = %zu\n",blockSize);
		int multiple = size / 16;						//return factor for multiple of 16
		paddingSize = 16*(multiple+1) - size;
		debug("PaddingSize = %zu\n",paddingSize);
		splinterSize = blockSize - (size + paddingSize + 16);	//16 is from header and footer
		debug("SplinterSize = %zu\n",splinterSize)


    	sf_header* freeBlockHdr = (sf_header*)freeBlockPtr;
    	void* freeBlockMem = ((char*)freeBlockHdr) + QWSIZE;
    	sf_footer* freeBlockFtr = (sf_footer*)((char*)freeBlockHdr + freeBlockSize - QWSIZE);

    	if(splinterSize < 32)
    	{
    		freeBlockHdr->splinter = 1;
    		freeBlockHdr->splinter_size = splinterSize;
    		freeBlockFtr->splinter = 1;
    	}
    	freeBlockHdr->alloc = 1;
    	freeBlockHdr->block_size = freeBlockSize >>4;
    	freeBlockHdr->padding_size = paddingSize;
    	freeBlockHdr->requested_size = size;
    	freeBlockFtr->alloc = 1;
    	freeBlockFtr->block_size = freeBlockSize>>4;


    	return freeBlockMem;
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
														//only keep block size in header and footer
	(hptr->alloc) = 0;									//set header alloc bit to 0;
	(fptr->alloc) = 0;									//set footer alloc bit to 0;
	(hptr->requested_size) = 0;
	(hptr->splinter) = 0;
	(hptr->padding_size) = 0;
	(hptr->splinter_size)= 0;
	(fptr->splinter) = 0;

	sf_footer* prevAdjacentBlockFt = (sf_footer*)(char*)hptr - QWSIZE;
	sf_header* nextAdjacentBlockHd = (sf_header*)(char*)fptr + QWSIZE;
	void* finalHd = NULL;

	if(prevAdjacentBlockFt->alloc == 0 && nextAdjacentBlockHd->alloc == 1)					//if only prev block is free
		finalHd = coalesce(prevAdjacentBlockFt,hptr);
	if(nextAdjacentBlockHd->alloc == 0 && prevAdjacentBlockFt->alloc == 1)					//if only adjacent block is free
		finalHd = coalesce(nextAdjacentBlockHd,hptr);
	if(prevAdjacentBlockFt->alloc == 0 && nextAdjacentBlockHd->alloc == 0)					//if both adjacent blocks are free
	{
		void* newHd = coalesce(prevAdjacentBlockFt,hptr);
		finalHd = coalesce(newHd,nextAdjacentBlockHd);
	}

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

	//coalesce(ptr);
	return;
}

int sf_info(info* ptr) {
	return -1;
}

void coalesce(void* ptr1, void* ptr2)
{

	return;
}

sf_free_header* bestFit(sf_free_header* freelist_head,size_t blockSize)
{
	sf_free_header* currptr = freelist_head;
	sf_free_header* smallestSize = NULL;
	sf_free_header* lastNodeFreeList = NULL;

while(smallestSize == NULL)
{
	while(currptr != NULL)
	{
		if(currptr->next == NULL && currptr->prev == NULL)										//only one big chunk of memory
			return currptr;
		if((currptr->header.block_size)== blockSize)
			return currptr;
		if(((currptr->header.block_size) > blockSize) && (smallestSize == NULL))	//set smallestSize to initial free block that is bigger than blockSize
			smallestSize = currptr;
		if(((currptr->header.block_size) > blockSize) &&
			(currptr->header.block_size) < (smallestSize->header.block_size))		//set smallestSize to freeBlock that is bigger than blockSize but smaller than smallestSize
			smallestSize = currptr;

		currptr = currptr->next;
		if(currptr->next == NULL)
			lastNodeFreeList = currptr;
	}

	if(smallestSize == NULL)														//freelist doesn't contain big enough size, make more space
	{
		void* ptrNewMem = sf_sbrk(4096);
		pages++;
		coalesce(lastNodeFreeList,ptrNewMem);										//coalesce the
	}
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
