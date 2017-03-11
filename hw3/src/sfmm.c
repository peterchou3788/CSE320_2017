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
#include <string.h>

#define QWSIZE 8		//Quadword = 64 bits = 8 bytes


void* coalesce(sf_header* ptr1);
sf_free_header* bestFit(sf_free_header* freelist_head,size_t requestedSize);
void init(sf_free_header* freelist_head,size_t alloc);
void removeFromFreelist(sf_free_header* freeHd);
void addToFreeList(void* head);
/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;

static size_t pages = 0;
static size_t allocatedBlocks = 0;
static size_t splinteredBlocks = 0;
static size_t paddingAmount = 0;
static size_t splinteringAmount = 0;
static size_t coalesceCounter = 0;
static double peakMemory = 0;
static size_t totalPayloadSizes = 0;

int addPages = 0;
size_t currAllocated = 0;
static size_t totalAllocated = 0;
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

        if(pages >4)
    	{
    		errno = ENOMEM;
    		return NULL;
    	}

        sf_header* hdptr = (sf_header*)freelist_head;							//address of header
        void* memptr = ((char*)freelist_head) + QWSIZE;							//address of payload
        sf_footer* ftptr = (sf_footer*)((char*)hdptr + blockSize - QWSIZE); 	//address of footer

        debug("%p\n",hdptr);
        debug("%p\n",memptr);
        debug("%p\n",ftptr);

        allocatedBlocks++;

        hdptr->alloc = 1;
        ftptr->alloc = 1;
        hdptr->block_size = blockSize >> 4;
        ftptr->block_size = blockSize >> 4;
        hdptr->requested_size = size;
        hdptr->padding_size = paddingSize;

        totalPayloadSizes+= size+paddingSize;
        peakMemory = totalPayloadSizes/((double)pages*4096.0);

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
    	{
    		errno = ENOMEM;
    		return NULL;
    	}
		size_t freeBlockSize = (freeBlockPtr->header.block_size )<< 4;								//in bytes
		size_t freeBlockSizeAfter = freeBlockSize - (blockSize);							//get block size of free block
		size_t payloadSize = freeBlockSize - 16;

    	debug("RequestedSize = %zu\n",size);
		//blockSize = freeBlockSize;						//make block size 16 addressable
		debug("BlockSize = %zu\n",freeBlockSize);
		int multiple = size / 16;						//return factor for multiple of 16
		int remainder = size % 16;
		paddingSize = 16*(multiple+1) - size;
		if(remainder == 0)
			paddingSize = 0;								//multiple of 16
		debug("PaddingSize = %zu\n",paddingSize);
		//splinterSize = freeBlockSize - ((size + paddingSize + 16));	//16 is from header and footer
		splinterSize = payloadSize - (size + paddingSize);
		debug("SplinterSize = %zu\n",splinterSize);

		allocatedBlocks++;

		paddingAmount += paddingSize;
		peakMemory = paddingAmount/((double)pages*4096.0);

    	sf_header* allocBlockHdr = (sf_header*)freeBlockPtr;
    	void* allocBlockMem = ((char*)allocBlockHdr) + QWSIZE;
    	sf_footer* allocBlockFtr = (sf_footer*)((char*)allocBlockHdr + (freeBlockSize) - QWSIZE);

    	if(splinterSize < 32)										//if splnter is less than 32, store splinter info
    	{
    		splinteredBlocks++;
			splinteringAmount+=splinterSize;
    		allocBlockHdr->splinter = 1;
    		allocBlockHdr->splinter_size = splinterSize;
    		allocBlockFtr->splinter = 1;
    		allocBlockHdr->alloc = 1;
    		allocBlockHdr->block_size = freeBlockSize>>4;
    		allocBlockHdr->padding_size = paddingSize;
    		allocBlockHdr->requested_size = size;
    		allocBlockFtr->alloc = 1;
    		allocBlockFtr->block_size = freeBlockSize>>4;
    		removeFromFreelist((sf_free_header*)allocBlockHdr);
    	}
    	else														//if splinter is bigger than 32, store the splitted free block into freelist
    	{
    		//sf_footer* freeBlockFtr = allocBlockFtr;
    		allocBlockFtr = (sf_footer*)((char*)allocBlockHdr + blockSize - QWSIZE);
    		allocBlockHdr->splinter = 0;
    		allocBlockHdr->splinter_size = 0;
    		allocBlockFtr->splinter = 0;
    		allocBlockHdr->alloc = 1;
    		allocBlockHdr->block_size = blockSize>>4;
    		allocBlockHdr->padding_size = paddingSize;
    		allocBlockHdr->requested_size = size;
    		allocBlockFtr->alloc = 1;
    		allocBlockFtr->block_size = blockSize>>4;
    																//update header for splitted free block
    		sf_header* freeBlockHdr = (sf_header*)((char*)allocBlockHdr + blockSize);
    		sf_footer* freeBlockFtr = (sf_footer*)((char*)freeBlockHdr + (freeBlockSizeAfter) - QWSIZE);

    		freeBlockHdr->alloc = 0;
    		freeBlockHdr->block_size = freeBlockSizeAfter>>4;
    		freeBlockHdr->padding_size = 0;
    		freeBlockHdr->requested_size = 0;
    		freeBlockHdr->splinter_size = 0;
    		freeBlockHdr->splinter = 0;
    		freeBlockFtr->alloc = 0;
    		freeBlockFtr->block_size = freeBlockSizeAfter>>4;
    		freeBlockFtr->splinter = 0;

    		//sf_blockprint(allocBlockHdr);
    		//sf_blockprint(freeBlockHdr);
    		removeFromFreelist((sf_free_header*)allocBlockHdr);



    		sf_header* checkFreeNextBlock = (sf_header*)((char*)freeBlockFtr + QWSIZE);
    		if(checkFreeNextBlock->alloc == 0 && checkFreeNextBlock != NULL)
    			addToFreeList(coalesce(freeBlockHdr));
    		else
    			addToFreeList(freeBlockHdr);
    	}


    	return allocBlockMem;
    }


}

void *sf_realloc(void *ptr, size_t size) {
	if(ptr == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	if(size == 0)
	{
		sf_free(ptr);
		return NULL;
	}

	sf_header* memheader = (sf_header*)((char*)ptr - QWSIZE);
	size_t memSize = memheader->block_size<<4;
	sf_footer* memfooter = (sf_footer*)((char*)memheader + memSize - QWSIZE);
	size_t payloadSize = memSize - 16;
	int multiple = size / 16;						//return factor for multiple of 16
	int remainder = size % 16;
	size_t paddingSize = 16*(multiple+1) - size;
	if(remainder == 0)
		paddingSize = 0;
	size_t newPayloadSize = 0;

	if(size == memSize)
		return ptr;

	if(size < memSize)										//moving to smaller size, use same block
	{
		newPayloadSize = payloadSize - (size + paddingSize);
		size_t splinterSize = payloadSize - newPayloadSize;

		if(splinterSize < 32)
		{
			memheader->splinter = 1;
			memheader->splinter_size = splinterSize;
			memheader->padding_size = paddingSize;
			memheader->requested_size = size;

			sf_header* nextAdjacentBlock = (sf_header*)((char*)memfooter +QWSIZE);
			if(nextAdjacentBlock->alloc == 0 && nextAdjacentBlock != NULL)
			{
				sf_footer* newfooter = (sf_footer*)((char*)memheader + newPayloadSize - QWSIZE);
				newfooter->alloc =1;
				newfooter->splinter = 0;
				newfooter->block_size = (newPayloadSize + 16)>>4;
				memheader->block_size = (newPayloadSize + 16)>>4;
				sf_header* splinterheader = (sf_header*)((char*)memheader + newPayloadSize);
				sf_header* newptrHd = (sf_header*)coalesce(splinterheader);
				sf_footer* newptrFt = (sf_footer*)newptrHd + (newptrHd->block_size<<4) -QWSIZE;

				newptrHd->splinter = 0;
				newptrHd->alloc = 0;
				newptrHd->requested_size = 0;
				newptrHd->padding_size = 0;
				newptrHd->splinter_size = 0;
				newptrFt->alloc = 0;
				newptrFt->splinter = 0;
				newptrFt->block_size = (newptrHd->block_size);
				addToFreeList((void*)newptrHd);
				return memheader;

			}

			else
			{
				memfooter->splinter = 1;
				return ptr;
			}
		}
		else														//if splinter is bigger than 32
		{
			sf_header* hdToFreeBlock = (sf_header*)((char*)memfooter + QWSIZE);
			if(hdToFreeBlock->alloc == 0 && hdToFreeBlock->block_size != 0)
    			removeFromFreelist((sf_free_header*)hdToFreeBlock);
    		sf_footer* allocBlockFtr = (sf_footer*)((char*)memheader + payloadSize - QWSIZE);
    		memheader->splinter = 0;
    		memheader->splinter_size = 0;
    		memheader->alloc = 1;
    		memheader->block_size = (newPayloadSize + 16)>>4;
    		memheader->padding_size = paddingSize;
    		memheader->requested_size = size;
    		allocBlockFtr->alloc = 1;
    		allocBlockFtr->block_size = (newPayloadSize + 16)>>4;
    		allocBlockFtr->splinter = 0;
    																//update header for splitted free block
    		sf_header* freeBlockHdr = (sf_header*)((char*)memheader + payloadSize);
    		sf_header* updated = (sf_header*)coalesce(freeBlockHdr);
    		sf_footer* freeBlockFtr = (sf_footer*)((char*)freeBlockHdr + (updated->block_size<<4) - QWSIZE);

    		freeBlockHdr->alloc = 0;
    		freeBlockHdr->block_size = updated->block_size;
    		freeBlockHdr->padding_size = 0;
    		freeBlockHdr->requested_size = 0;
    		freeBlockHdr->splinter_size = 0;
    		freeBlockHdr->splinter = 0;
    		freeBlockFtr->alloc = 0;
    		freeBlockFtr->block_size = updated->block_size;
    		freeBlockFtr->splinter = 0;

    		//sf_blockprint(allocBlockHdr);
    		//sf_blockprint(freeBlockHdr);


    		addToFreeList((void*)freeBlockHdr);
    		return ptr;
    	}

		}


	else															//reallocing to bigger size
	{
		sf_header* headOfNextBlock = (sf_header*)(char*)memfooter + QWSIZE;		//check if next block is free so can coalesce
		if(headOfNextBlock->alloc == 0 && headOfNextBlock != NULL)
		{
			removeFromFreelist((sf_free_header*)headOfNextBlock);
			sf_header* newptrHd = (sf_header*)coalesce(memheader);
			sf_footer* newptrFt = (sf_footer*)((char*)newptrHd + (newptrHd->block_size<<4) - QWSIZE);
			newptrHd->alloc = 0;
			newptrHd->requested_size=0;
			newptrHd->splinter_size=0;
			newptrHd->splinter=0;
			newptrHd->padding_size=0;
			newptrFt->alloc = 0;
			newptrFt->splinter = 0;
			addToFreeList(newptrHd);

			if((newptrHd->block_size)>(size + paddingSize + 16))
			{
				splinterSize = (newptrHd->block_size) - size -16 - paddingSize;
				if(splinterSize <32)
				{
					newptrHd->splinter = 1;
					newptrFt->splinter = 1;
					newptrHd->splinter_size = splinterSize;
					newptrHd->alloc = 1;
					newptrHd->requested_size= size;
					newptrHd->padding_size=paddingSize;
					newptrFt->alloc = 1;
					return (void*)(newptrHd+QWSIZE);
				}
				else
				{
					sf_header* splittedBlockHd = (sf_header*)((char*)newptrHd + (newptrHd->block_size<<4));
					sf_footer* allocatFt = (sf_footer*)((char*)newptrHd + size+paddingSize+16 - QWSIZE);

					allocatFt->splinter = 0;
					allocatFt->block_size = size + paddingSize + 16;
					allocatFt->alloc = 1;
					newptrHd->block_size = allocatFt->block_size;

					splittedBlockHd->alloc=0;
					splittedBlockHd->requested_size=0;
					splittedBlockHd->splinter_size=0;
					splittedBlockHd->splinter=0;
					splittedBlockHd->padding_size=0;
					splittedBlockHd->block_size = splinterSize + 16;

					newptrFt->alloc = 0;
					newptrFt->splinter = 0;
					newptrFt->block_size = splinterSize + 16;

					addToFreeList(splittedBlockHd);

					return (void*)(newptrHd+QWSIZE);
				}

			}

		}

			sf_free_header* bestfitptr = (sf_free_header*)(bestFit(freelist_head,size+16+paddingSize));
			sf_header* bestfitHd = (sf_header*)bestfitptr;
			void* bestfitMem = (void*)((char*)bestfitHd + QWSIZE);
			sf_footer* bestfitFt = (sf_footer*)bestfitHd + (bestfitHd->block_size<<4) - QWSIZE;
			splinterSize = (bestfitHd->block_size) - 16 - size - paddingSize;
			if(splinterSize<32)
			{
				bestfitHd->splinter = 1;
				bestfitFt->splinter = 1;
				bestfitHd->splinter_size = splinterSize;
			}
			bestfitHd->alloc = 1;
			bestfitHd->requested_size = size;
			bestfitHd->padding_size = paddingSize;
			bestfitFt->alloc = 1;
			memcpy(bestfitMem,ptr,payloadSize);
			return bestfitMem;

	}

	return NULL;
}

void sf_free(void* ptr) {
	//char* header = ((char*)ptr) - QWSIZE;
	//char* footer = NULL;
	sf_header* hptr = (sf_header*)((char*)ptr - QWSIZE);
	size_t size = ((hptr->block_size) <<4);
	sf_footer* fptr = (sf_footer*)((char*)hptr + size - QWSIZE);

	if((size != (fptr->block_size <<4)) || (hptr->alloc == 0) || (hptr == NULL))	//if header alloc does not match with footer address is invalid
	{
		errno = EINVAL;
		return;
	}
	splinteringAmount-= hptr->splinter_size;
	paddingAmount-= hptr->padding_size;
	peakMemory = paddingAmount/((double)pages * 4096.0);

														//only keep block size in header and footer
	(hptr->alloc) = 0;									//set header alloc bit to 0;
	(fptr->alloc) = 0;									//set footer alloc bit to 0;
	(hptr->requested_size) = 0;
	(hptr->splinter) = 0;
	(hptr->padding_size) = 0;
	(hptr->splinter_size)= 0;
	(fptr->splinter) = 0;

	void* finalHd = coalesce(hptr);
	addToFreeList(finalHd);						//put free block into freelist
	allocatedBlocks--;
	splinteredBlocks--;


	return;
}

int sf_info(info* ptr) {
	if(ptr == NULL && pages == 0)
		return -1;

	ptr->splinterBlocks = splinteredBlocks;
	ptr->allocatedBlocks = allocatedBlocks;
	ptr->padding  = paddingAmount;
	ptr->coalesces = coalesceCounter;
	ptr->splintering = splinteringAmount;
	ptr->peakMemoryUtilization = peakMemory;

	return 0;
}

void* coalesce(sf_header* ptr1)
{
	sf_header* headerToCoalesce = ptr1;
	sf_footer* footerToCoalesce = (sf_footer*)((char*)ptr1 + (ptr1->block_size<<4) - QWSIZE);
	sf_footer* prevAdjacentBlockFt = (sf_footer*)((char*)headerToCoalesce - QWSIZE);
	sf_header* nextAdjacentBlockHd = (sf_header*)((char*)footerToCoalesce + QWSIZE);
	size_t newBlockSize = 0;
	sf_header* newHd = NULL;
	sf_footer* newFt = NULL;

	if((prevAdjacentBlockFt->alloc == 0 && nextAdjacentBlockHd->alloc == 1) || nextAdjacentBlockHd == NULL)					//if only prev block is free
	{
		if((prevAdjacentBlockFt->block_size) == 0)
			return (void*)ptr1;
		newBlockSize = (prevAdjacentBlockFt->block_size<<4) + (headerToCoalesce->block_size<<4);
		newHd = (sf_header*)((char*)prevAdjacentBlockFt - (prevAdjacentBlockFt->block_size<<4) + QWSIZE);
		newFt = footerToCoalesce;
		removeFromFreelist((sf_free_header*)newHd);
		coalesceCounter++;
	}
	if(nextAdjacentBlockHd->alloc == 0 && prevAdjacentBlockFt->alloc == 1)					//if only adjacent block is free
	{
		if(nextAdjacentBlockHd->block_size == 0)
			return (void*)ptr1;
		newBlockSize = (headerToCoalesce->block_size<<4) + (nextAdjacentBlockHd->block_size<<4);
		newHd = headerToCoalesce;
		newFt = (sf_footer*)((char*)nextAdjacentBlockHd + (nextAdjacentBlockHd->block_size<<4) - QWSIZE);
		removeFromFreelist((sf_free_header*)newHd);
		coalesceCounter++;
	}
	if(prevAdjacentBlockFt->alloc == 0 && nextAdjacentBlockHd->alloc == 0)					//if both adjacent blocks are free
	{
		newBlockSize = (headerToCoalesce->block_size<<4) + (prevAdjacentBlockFt->block_size<<4) + (nextAdjacentBlockHd->block_size<<4);
		if((prevAdjacentBlockFt->block_size == 0) && (nextAdjacentBlockHd->block_size == 0))		//free blocks adjacent but no block size
			return (void*)ptr1;

		if((prevAdjacentBlockFt->block_size != 0) && (nextAdjacentBlockHd->block_size == 0))
		{
			newHd = (sf_header*)((char*)prevAdjacentBlockFt - (prevAdjacentBlockFt->block_size<<4) + QWSIZE);
			newFt = footerToCoalesce;
			removeFromFreelist((sf_free_header*)newHd);
		}
		if((prevAdjacentBlockFt->block_size == 0) && (nextAdjacentBlockHd->block_size != 0))
		{
			newHd = headerToCoalesce;
			newFt = (sf_footer*)((char*)nextAdjacentBlockHd + (nextAdjacentBlockHd->block_size<<4) - QWSIZE);
			removeFromFreelist((sf_free_header*)((char*)prevAdjacentBlockFt -(prevAdjacentBlockFt->block_size<<4) +QWSIZE));
		}
		else
		{
			newHd = (sf_header*)((char*)prevAdjacentBlockFt - (prevAdjacentBlockFt->block_size<<4) + QWSIZE);
			newFt = (sf_footer*)((char*)nextAdjacentBlockHd + (nextAdjacentBlockHd->block_size<<4) - QWSIZE);
			removeFromFreelist((sf_free_header*)nextAdjacentBlockHd);
			removeFromFreelist((sf_free_header*)newHd);
		}

		newHd->block_size = newBlockSize>>4;
		newFt->block_size = newBlockSize>>4;
		coalesceCounter++;
	}
	else																					//no free blocks adjacent so return original pointer
		newHd = ptr1;


	return (void*)newHd;
}

sf_free_header* bestFit(sf_free_header* freelist_head,size_t blockSize)
{
	sf_free_header* currptr = freelist_head;
	sf_free_header* smallestSize = NULL;
	//sf_free_header* lastNodeFreeList = NULL;

while(smallestSize == NULL)
{
	while(currptr != NULL)
	{
		if(currptr->next == NULL && currptr->prev == NULL)										//only one big chunk of memory
			return currptr;
		if((currptr->header.block_size<<4 )== blockSize)
			return currptr;
		if(((currptr->header.block_size<<4) > blockSize) && (smallestSize == NULL))	//set smallestSize to initial free block that is bigger than blockSize
			smallestSize = currptr;
		if(((currptr->header.block_size<<4) > blockSize) &&(currptr->header.block_size>>4) < (smallestSize->header.block_size>>4))		//set smallestSize to freeBlock that is bigger than blockSize but smaller than smallestSize
			smallestSize = currptr;

		/*if(currptr->next == NULL)
			lastNodeFreeList = currptr; */
		currptr = currptr->next;
	}

	if(smallestSize == NULL)														//freelist doesn't contain big enough size, make more space
	{
		int getpages = (blockSize/4096) + 1;
		void* ptrNewMem = sf_sbrk(blockSize);
		init((sf_free_header*)ptrNewMem, getpages * 4096);
		pages += getpages;
		smallestSize = coalesce(ptrNewMem);														//coalesce the new memory accordingly
		addToFreeList(smallestSize);

	}
}
	//removeFromFreelist(smallestSize);
	return smallestSize;
}

void init(sf_free_header* freeptr, size_t alloc)								//gives header and footer to free block
{
	sf_header *head = (sf_header*)freeptr;
	sf_footer *footer = (sf_footer*)(((char*)freeptr) + alloc - QWSIZE);
	head->block_size = alloc>>4;
	footer->block_size = alloc>>4;

	return;
}

void removeFromFreelist(sf_free_header* freeHd)
{
	sf_free_header* prevNode = freeHd->prev;
	sf_free_header* nextNode = freeHd->next;

	if(prevNode == NULL && nextNode == NULL)
	{
		freelist_head = NULL;
		return;
	}
	if(prevNode == NULL && nextNode != NULL)
	{
		nextNode->prev = NULL;
		return;
	}
	if(prevNode != NULL && nextNode == NULL)
	{
		prevNode->next = NULL;
		return;
	}
	else
	{

		prevNode->next = nextNode;
		nextNode->prev = prevNode;

		return;
	}
}

void addToFreeList(void* ptr1)
{
	sf_free_header* freeHd = (sf_free_header*)ptr1;
	if(freelist_head == NULL)
	{
		freelist_head = freeHd;
		return;
	}
	sf_free_header* freelistCurr = freelist_head;


	if(freeHd < freelist_head)
	{
		sf_free_header* temp = freelist_head;
		freeHd->next = temp;
		temp->prev = freeHd;
		freelist_head = freeHd;
		return;

	}
	else
	{
		while(freelistCurr != NULL)
		{
			if(((freeHd) > (freelistCurr)) && (freelistCurr->next == NULL))	//end of freelist
			{
				freeHd->prev = freelistCurr;			//set prev of free Block to current node in freelist
				freeHd->next = NULL;					//set next of free Block  NULL
				freelistCurr->next = freeHd;			//set current node's next to free Block
				return;
			}
			if(((freeHd) > (freelistCurr) )&& ((freeHd) < (freelistCurr->next)))
			{
				freeHd->prev = freelistCurr;			//set prev of free Block to current node in freelist
				freeHd->next = freelistCurr->next;		//set next of free Block to next node of current in freelist
				freelistCurr->next = freeHd;			//set current node's next to free Block
				freelistCurr->next->prev = freeHd;		//set current node's next's prev to freeBlock
				return;
			}
			freelistCurr = freelistCurr->next;
		}


	}

}

