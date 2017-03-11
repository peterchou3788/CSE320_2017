#include <criterion/criterion.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfmm.h"

/**
 *  HERE ARE OUR TEST CASES NOT ALL SHOULD BE GIVEN STUDENTS
 *  REMINDER MAX ALLOCATIONS MAY NOT EXCEED 4 * 4096 or 16384 or 128KB
 */

Test(sf_memsuite, Malloc_an_Integer, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(sizeof(int));
  *x = 4;
  cr_assert(*x == 4, "Failed to properly sf_malloc space for an integer!");
}

Test(sf_memsuite, Free_block_check_header_footer_values, .init = sf_mem_init, .fini = sf_mem_fini) {
  void *pointer = sf_malloc(sizeof(short));
  sf_free(pointer);
  pointer = (char*)pointer - 8;
  sf_header *sfHeader = (sf_header *) pointer;
  cr_assert(sfHeader->alloc == 0, "Alloc bit in header is not 0!\n");
  sf_footer *sfFooter = (sf_footer *) ((char*)pointer + (sfHeader->block_size << 4) - 8);
  cr_assert(sfFooter->alloc == 0, "Alloc bit in the footer is not 0!\n");
}

Test(sf_memsuite, SplinterSize_Check_char, .init = sf_mem_init, .fini = sf_mem_fini){
  void* x = sf_malloc(32);
  void* y = sf_malloc(32);
  (void)y;

  sf_free(x);

  x = sf_malloc(16);

  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->splinter == 1, "Splinter bit in header is not 1!");
  cr_assert(sfHeader->splinter_size == 16, "Splinter size is not 16");

  sf_footer *sfFooter = (sf_footer *)((char*)sfHeader + (sfHeader->block_size << 4) - 8);
  cr_assert(sfFooter->splinter == 1, "Splinter bit in header is not 1!");
}

Test(sf_memsuite, Check_next_prev_pointers_of_free_block_at_head_of_list, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(4);
  memset(x, 0, 0);
  cr_assert(freelist_head->next == NULL);
  cr_assert(freelist_head->prev == NULL);
}

Test(sf_memsuite, Coalesce_no_coalescing, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(4);
  int *y = sf_malloc(4);
  memset(y, 0, 0);
  sf_free(x);

  //just simply checking there are more than two things in list
  //and that they point to each other
  cr_assert(freelist_head->next != NULL);
  cr_assert(freelist_head->next->prev != NULL);
}

//#
//STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
//DO NOT DELETE THESE COMMENTS
//#

Test(sf_memsuite, sf_info_using, .init = sf_mem_init, .fini = sf_mem_fini)
{
    info* infoptr = (info*)sf_malloc(sizeof(info*));
    int *x = sf_malloc(sizeof(int));
    memset(x,0,0);
    //*x = 4;
    double* y = sf_malloc(sizeof(double));
    //*y = 6.0;
    //printf("%f\n",*y);
    sf_free(y);
   // printf("%d\n",*x);

    sf_info(infoptr);

    cr_assert((infoptr->allocatedBlocks)= 2, "Allocated Blocks Not 2\n");
    cr_assert((infoptr->coalesces) = 1, "Coealsce not 1\n");
    cr_assert((infoptr->padding) = 20, "Padding not 20\n");
}

Test(sf_memsuite, Malloc_a_char, .init = sf_mem_init, .fini = sf_mem_fini)
{
  char* ch = sf_malloc(sizeof(char));
  *ch = 'd';

  cr_assert(*ch == 'd' ,"Failed To make space for a character");
}

Test(sf_memsuite,Freeing_Mem, .init = sf_mem_init, .fini = sf_mem_fini)
{
  int* integer = sf_malloc(sizeof(int));
  //*integer = 5;

  sf_free(integer);

  sf_header* header = (sf_header*)((char*)integer - 8);
  sf_footer* footer = (sf_footer*)((char*)header + (header->block_size<<4) - 8);
  cr_assert((header->alloc) == 0, "Header alloc not 0\n");
  cr_assert((footer->alloc) == 0, "Footer alloc not 0\n");
}


