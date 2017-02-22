# CSE320 Spring 2017 - Homework 3

#### **Due Friday 03/10/2017 @ 11:59PM**

### We HIGHLY suggest that you read this entire document, the book chapter, and examine the base code prior to beginning.

### If you do not read the entire document before beginning, then you may find yourself doing extra work!

> :scream: Start early so that you have an adequate amount of time to test your program!

# Introduction

For this assignment, you will write a dynamic memory allocator as an alternative for the built-in one provided by [glibc](https://en.wikipedia.org/wiki/GNU_C_Library).

You must read **Chapter 9.9 Dynamic Memory Allocation** before starting this assignment. This chapter contains all the theoretical information needed to complete this assignment. Since the textbook has sufficient information about the different design strategies and implementation details of an allocator, this document will not cover this information. Instead, it will refer you to the necessary sections and pages in the textbook.

## Takeaways

After completing this assignment you will have more experience with or a better understanding of:
* The inner working of a dynamic memory allocator
* What the different types of fragmentation are and when they occur
* Memory Padding and Alignment
* Structs and Linked Lists in C
* [ERRNO](https://linux.die.net/man/3/errno) numbers in C
* Unit Testing in C

# Your Allocator

You will create an explicit free list allocator with best-fit placement, immediate coalescing, and block splitting without creating splinters.

Free blocks will be inserted into the list in **sorted ascending address order**.

You will implement your own versions of the *malloc*, *realloc*, and *free* functions.

You will use existing Criterion unit tests and write your own to help debug your implementation.

## Explicit Freelist Management Policy

You **MUST** use an **explicit freelist**, as described in **Chapter 9.9.13 Page 862**, to manage free blocks.

Your allocator will manage your freelist by maintaining the allocations in increasing address order (low address in memory to high address in memory).

## Block Placement Policy

Your allocator will support the **best fit placement** policy, as described in **Chapter 9.9.7 Page 849**.

> :thinking: If there are multiple free blocks of the **same size**, that can store the **requested size**, the **best fit placement** policy should choose the block that has the lower address.

## Immediate Coalescing

Your implementation must perform **immediate coalescing** as described in **Chapter 9.9.10 Page 850**. You must use boundary tags as described in **Chapter 9.9.11 Page 851** to assist in coalescing all free memory blocks in an efficient manner.

In **Chapter 9.9.11 Page 852 Figure 9.39**, the boundary tag (footer) is defined as a single 32-bit double word with the block size and allocated bit. Your boundary tag must be a single memory row of 64-bits in a similar format.

<!--- FIXME: (or not) FANCY TABLE
<table style="width:50%">
<tr>
<td>             unused             </td><td>         block_size         </td><td align=center><tt>0s</tt><td align=center><tt>0a</tt>
</tr>
</table>
-->

## Splitting Blocks & Splinters

Your allocator must split blocks which are too large to help reduce the amount of internal fragmentation. Details about this feature can be found in **Chapter 9.9.8 Page 849**.

When splitting blocks your allocator **MUST NOT** create splinters. Splinters are small (\<32 bytes in this case) groups of free bytes left after inserting a relatively large payload into free space. To avoid this your allocator must "over" allocate the amount of memory requested so that these small useless blocks are not inserted into the free list.

## Block Headers & Footers

In Chapter 9.9.6 Page 847 Figure 9.35, the header is defined as a single 32-bit double word with the block size and allocated bit.

In this assignment the block header is defined as a single 64-bit quadword, similar to the header in the textbook. However the header for this assignment has two extra fields. One for recording the number of bytes allocated for padding, and one for recording the number of extra bytes allocated to prevent splintering.

> **Note:** 1 full memory row is 64-bits in size (8 bytes = 1 quad word in x86_64)

**Block Header Format:**
>
    +-------------+--------------+---------+---------------+------------+---------+-------+
    | padding_size| splinter_size| unused  | requested_size| block_size | splinter| alloc |
    |   in bytes  |    in bytes  |  9 bits |   in bytes    |   in bytes |  0s     |  0a   |
    |   4 bits    |    5 bits    |         |   14 bits     |   28 bits  | 2 bits  | 2 bits|
    +-------------+--------------+---------+---------------+------------+---------+-------+



**Block Footer Format:**
>

    +------------------------------------------------------+------------+----------+---------+
    |                         unused                       | block_size | splinter |  alloc  |
    |                                                      |  in bytes  |    0s    |   0a    |
    |                        32 bits                       |   28 bits  |  2 bits  | 2 bits  |
    +------------------------------------------------------+------------+---------+-------+

- The `padding_size` field is 4-bits wide. It represents the number of bytes used for padding by the allocator. It is only 4 bits wide because at most only 15 bytes are required for alignment.
- The `splinter_size` field is 5-bits wide. It represents the number of extra bytes allocated to prevent a splinter. It is only 5 bits wide because at most the size of a splinter is 31 bytes.
- The `unused` field represents bits in the header that are not needed or used by your allocator.
- The `requested_size` field represents the size that was requested by the user to `sf_malloc`.
- The `block_size` field is 28 bits. It is the number of bytes for the entire block (header/footer, payload, padding).
- The least-significant bit of the `splinter` field represents whether or not the memory block contains a splinter.
- The least-significant bit is the `allocated` bit (as explained in the textbook).

> :thinking: The block size field is only 28 bits but technically it "uses" the 0s and 0a bits as part of the value. It just assumes these bits are always 0 because the address must be divisible by 16 (alignment for largest type in x86_64). This gives us an actual representation of 32 bits for the block size.
> For example, if the requested is 13 bytes and the block size is 32 bytes the header will be set to: `001100000xxxxxxxxx0000000000110100000000000000000000000000100001`

> :thinking: Another example, would be lets if the requested size is 32 bytes and we found a free block with a block size of 64 bytes. The requested size would be 32 bytes, splinter would be set to 1 byte, splinter size would be 16 bytes, and block size would be 64 bytes. Then the hex representation of the header would be as follows: `000010000xxxxxxxxx0000000010000000000000000000000000000001000101`
> Notice the header will always be at an address which is a mutiple of 8 (but not 16) due to payload alignment. This allows us to use the LSB (least significant bit) as a flag for the allocated state and the splinter state of the block.




# Getting Started

Fetch and merge the base code for `hw3` as described in `hw0` from the following link: [https://gitlab01.cs.stonybrook.edu/cse320/hw3](https://gitlab01.cs.stonybrook.edu/cse320/hw3).

## Directory Structure

<pre>
hw3
repo/hw3
├── build
│   └── sfutil.o
├── include
│   ├── debug.h
│   └── sfmm.h
├── Makefile
├── src
│   ├── main.c
│   └── sfmm.c
└── tests
    └── tests.c
</pre>


The base\_code  includes the `build` folder in the `hw3` directory.

* The `build` folder contains the object file for the `sfutil` library. This library provides you with several functions to aid you with the implementation of your allocator.
    * <span style="color:red">**Do NOT delete this file as it is an essential part of your homework assignment.**</span>
    * Note: `make clean` will not delete `sfutil.o` or the 'build' folder, but it will delete all other contained `.o` files.

All functions for your allocator (`sf_malloc`, `sf_free`, and `sf_realloc`) must be implemented in `src/sfmm.c`. There is a small sample main file ( `src/main.c`) which can be used to test your allocator. After your allocator passes these **very basic tests**, you will create your own tests.

The provided `Makefile` creates object files from the `.c` files in the `src` directory, places the object files inside the `build` directory, and then links the object files together to make executables, located in the `bin` directory.

> :scream: The functions `malloc`, `free`, `realloc`, `memalign`, `calloc`, etc., are **NOT ALLOWED** in your implementation. If any of these functions, or any other function with similar functionality is found in your program, you **will receive a <font color="red">ZERO</font>**.

> Any main program(s) provided or which you create **WILL NOT** be graded. Your Makefile will also be replaced for grading. Any changes to either of these files will not be graded.

> **DO NOT modify the `sfmm.h` header file.** We will replace this file when we perform tests for grading. If you wish to add things to a header file, please create a new header file in the `include` folder.

# Allocation Functions

You will implement the following three functions in the file `src/sfmm.c`. The file `include/sfmm.h` contains the prototypes and documentation found here.

```c
/*
 * This is your implementation of malloc. It acquires uninitialized memory that
 * is aligned and padded properly for the underlying system.
 *
 * @param size The number of bytes requested to be allocated.
 *
 * @return If successful, the pointer to a valid region of memory of the
 * requested size is returned, else NULL is returned and ERRNO is set to ENOMEM.
 *
 * If size is zero, then NULL is returned and ERRNO is set to EINVAL.
 */
void *sf_malloc(size_t size);

/*
 * Resizes the memory pointed to by ptr to size bytes.
 *
 * @param ptr Address of the memory region to resize.
 *
 * @param size The minimum size to resize the memory to.
 *
 * @return If successful, the pointer to a valid region of memory is
 * returned, else NULL is returned and ERRNO is set appropriately.
 *
 * If there is no memory available sf_malloc should set ERRNO to ENOMEM.
 *
 * If sf_realloc is called with an invalid pointer ERRNO should be set to EINVAL.
 *
 * If sf_realloc is called with a valid pointer and a size of 0 it should free the
 * allocated block and return NULL.
 */
void* sf_realloc(void *ptr, size_t size);

/*
 * Marks a dynamically allocated region as no longer in use.
 * Adds the newly freed block to the free list.
 *
 * @param ptr Address of memory returned by the function sf_malloc.
 * If the address of the memory being freed is not valid, this
 * function sets ERRNO to EINVAL.
 */
void sf_free(void *ptr);
```
> :scream: <font color="red">Make sure these functions have these exact names and arguments. They must also appear in the correct file. If you do not name the functions correctly with the correct arguments, your program will not compile when we test it. **YOU WILL GET A ZERO**</font>


# Initialization Functions

In the `build` directory we have provided you with the `sfutil.o` object file. When linked with your program, this object file allows you to use the `sfutil` library.

> :scream: As these functions are provided to you in a pre-built .o file, the source is not available and you will not be able to debug using gdb inside these functions. You must treat them as black boxes.

This library contains the following functions:
```c
/*
 * This function must be called in your program's implementation ONCE,
 * before the use of any of your allocator functions.
 * This function DOES NOT allocate any space to your allocator.
 */
void sf_mem_init();

/*
 * This routine will finalize your memory allocator. It should be called
 * in your program once, prior to exiting your program.
 * If implemented cleanly, your program should have no memory leaks in
 * valgrind after this function is called.
 */
void sf_mem_fini();

/*
 * This function changes the position of your program's break.
 * Calling sf_sbrk(0) can be used to find the current location of the heap.
 *
 * @param increment The number of bytes requested.
 * If 0 sf_sbrk will return current location of the heap,
 * Otherwise the program's break will be incremented by ceil(increment / 4096) * 4096.
 *
 * @return On success, sf_sbrk(size_t increment) returns the previous program break.
 * (If the break was increased, then this value is a pointer to the
 * start of the newly allocated memory). On error, (void *) -1 is returned,
 * and errno is set to ENOMEM.
 */
void *sf_sbrk(size_t increment);
```

The functions `sf_mem_init` and `sf_sbrk` MUST be used to initialize and request memory for your allocator when it is needed.


## sf_sbrk

This is the function that **sf_malloc** must call to request heap space for your allocator. Its operation is similar to the real system call **sbrk**.

At some point in your program, you may want to find where the heap breakpoint currently is. To find the current position of the heap breakpoint, you can call **sf_sbrk** with a size of zero and it will return the address of the current heap breakpoint (without moving it).

If **sf_sbrk** is unable to return the requested size of memory you asked for, it will return **`(void *) -1`**, and **`errno`** will be set to **`ENOMEM`**, which is same as `sbrk`.

> :scream: Your implementation will manage a maximum of 4 pages of memory (1 page = 4096 bytes). Any requests which exceeds this are considered an **ERROR** (return NULL).

> :thinking: If there is a free block at the end of the heap and the heap is extended, your allocator should coalesce this block with the newly allocated space.
>
### sf_sbrk warnings

For this assignment, your implementation **MUST ONLY** use `sf_sbrk` to move the heap breakpoint. DO NOT use any system calls such as **brk** or **sbrk** to move the heap breakpoint.

> :smile: A real allocator may use the **brk**/**sbrk** syscalls for small memory allocations and **mmap**/**munmap** syscalls for large allocations. To allow your program to use other functions provided by glibc, that rely on glibc's allocator, we prove a safe wrapper around **sbrk**. This makes it so that the heap breakpoint does not get altered by glibc's allocator and destroy the memory managed by your allocator.


# Implementation Details

## Memory Row Size

In this assignment we will assume that each "Memory Row" is 64-bits in size (1 quad word in x86_64).

The table below lists the sizes of date types on x86-64 Linux Mint:

| C declaration | Intel data type | GAS suffix | x86-64 Size (Bytes) |
| :--------------: | :----------------: | :-----------: | :----------------------: |
| char  | Byte | b | 1 |
| short | Word | w | 2 |
| int   | Double word | l | 4 |
| unsigned | Double word |  l | 4 |
| long int | Quad word | q | 8 |
| unsigned long | Quad word | q | 8 |
| pointer | Quad word | q | 8 |
| float | Single precision | s | 4 |
| double | Double precision | d | 8 |
| long double | Extended precision | t | 16 |

Consider this information when determining minimum block sizes and block alignment.

## Freelist head

In the file `src/sfmm.c` there is a pointer declared `freelist_head`, which is made available globally by the `extern` keyword in `sfmm.h`.

```c
/*
* You must store the head of your free list in this variable.
* Doing so will make it accessible via the extern keyword.
* This will allow sf_snapshot to access the value from a different
* file.
*/
extern sf_free_header* freelist_head = NULL;
```
> :scream: You MUST use this pointer to access the head of your explicit freelist. The helper functions discussed later as well as the unit test use this variable to access your explicit free list.

## Block Headers & Footer

The block header and footers formats and field sizes are specified in `include/sfmm.h`.

```c
#define PADDING_SIZE_BITS 4
#define SPLINTER_SIZE_BITS 5
#define UNUSED_SIZE_BITS 9
#define REQUEST_SIZE_BITS 14
#define BLOCK_SIZE_BITS 28
#define SPLINTER_CONSUMED_BITS 2
#define ALLOC_SIZE_BITS 2

#define SF_HEADER_SIZE \
    ((ALLOC_SIZE_BITS + BLOCK_SIZE_BITS + UNUSED_SIZE_BITS + \
      PADDING_SIZE_BITS + REQUEST_SIZE_BITS + SPLINTER_SIZE_BITS + SPLINTER_CONSUMED_BITS) >> 3)

#define SF_FOOTER_SIZE SF_HEADER_SIZE

/*

                                      Format of a memory block
    +--------------------------------------------------------------------------------------+
    |                                       64-bits wide                                   |
    +--------------------------------------------------------------------------------------+
    +-------------+--------------+---------+---------------+---------------+-------+-------+
    | Padding Size| Splinter Size| _Unused | Requested Size| Block Size    |   0s  | 0a    | <- Header Block
    |   in bytes  |    in bytes  |  9 bits |   in bytes    |   in bytes    |       |       |
    |   4 bits    |    5 bits    |         |   14 bits     |    28 bits    | 2 bits| 2 bits|
    +-------------+--------------+---------+---------------+---------------+-------+-------+
    |                                                                                      |
    |                             Payload and Padding                                      |
    |                                (N Memory Rows)                                       |
    |                                                                                      |
    |                                                                                      |
    +------------------------------------------------------+---------------+-------+-------+
    |                     _Unused                          |  Block Size   |   0s  |   0a  | <- Footer Block
    +------------------------------------------------------+---------------+-------+-------+
*/

```
The file contains the general format for memory blocks in your allocator.  The block size is defined as:

![alt text](http://i.imgur.com/eaBh9pA.png)

The block header is defined as:

```c
struct __attribute__((__packed__)) sf_header {
    uint64_t          alloc : ALLOC_SIZE_BITS;
    uint64_t       splinter : SPLINTER_CONSUMED_BITS;
    uint64_t     block_size : BLOCK_SIZE_BITS;
    uint64_t requested_size : REQUEST_SIZE_BITS;
    uint64_t         unused : UNUSED_SIZE_BITS;
    uint64_t  splinter_size : SPLINTER_SIZE_BITS;
    uint64_t   padding_size : PADDING_SIZE_BITS;
};
typedef struct sf_header sf_header;
```
> :smile: The  `__attribute__((__packed__))` tells gcc to leave out all padding between members of the struct. In this way, the fields are forcibly placed next to each other.

To assist in accessing the next and free pointers of a free block, an additional struct `sf_free_header` is defined.

```c
struct __attribute__((__packed__)) sf_free_header {
    sf_header header;
    struct sf_free_header *next;
    struct sf_free_header *prev;
};
typedef struct sf_free_header sf_free_header;
```

Lastly, `sfmm.h` also provides a struct for representing the footer.

```c
struct __attribute__((__packed__)) sf_footer {
    uint64_t      alloc : ALLOC_SIZE_BITS;
    uint64_t   splinter : SPLINTER_CONSUMED_BITS;
    uint64_t block_size : BLOCK_SIZE_BITS;
    /* Other 32-bits are unused */
};
typedef struct sf_footer sf_footer;
```

:thumbsup: You can cast blocks of memory with these structs in order to easily access the different fields of information of your memory allocator blocks.

## Notes on sf_realloc

When implementing your `sf_realloc` function, consider several different cases. `sf_realloc` **SHOULD NOT** call `sf_malloc` to allocate more memory, call `memcpy` to move the old memory to the new memory, and call `sf_free` to free the old memory.

There are situations that arise when reallocing that you must be wary of when implementing the function.

> :thinking: What should happen when you realloc a memory block and there are leftovers? What about **splinters**? What if there is an adjacent block in memory that is free?

```
When reallocing a block to a smaller size, your allocator may be able to
split this block and insert this newly split block into the free list.

            b                                        b
+----------------------+                       +----------------------+
| allocated            |                       | allocated |  free    |
| Blocksize: 64 bytes  |   sf_realloc(b, 16)   | 32 bytes  |  32 bytes|
| payload: 48 bytes    |                       | payload:  |          |
|                      |                       | 16 bytes  | goes into|
|                      |                       |           | free list|
+----------------------+                       +----------------------+
```

```

When reallocing an allocated block to a smaller size, splitting the block may result
in a splinter.

If this is the case your allocator should check if there is
an adjacent free block in memory and coalesce the splinter with that free block.

If there is no adjacent free block in memory your allocator should keep not split 
the block and update the header fields to indicate that this block contains a splinter.

            b                                        b     *****************************
+----------------------+                       +-----------*----------+ +------------+ *
| allocated            |                       | allocated *  free    |-|  free      | *
| Blocksize: 48 bytes  |   sf_realloc(b, 16)   | 32 bytes  *  16 bytes|-|  32 bytes  | *
| payload: 32 bytes    |                       | payload:  *          |-|            | *
|                      |                       | 16 bytes  * goes into|-|  adjacent  | *
|                      |                       |           * free list|-| in memory  | *
+----------------------+                       +-----------*----------+ +------------+ *
                                                           *****************************
                                          *Coalesce with adjacent free block and put into free list
```

```

When reallocing a block to a larger size, your allocator should check if there is an
adjacent free block in memory.

If this is the case, coalesce the two blocks and split if possible.

If there is no adjacent free block in memory, move the data to a new block and free the
old block.

            b                                                      b
                                               ********************************************
+----------------------+                       * +----------------------+ +-------------+ *
| allocated            |                       * |  allocated           |-|  allocated  | *
| Blocksize: 48 bytes  |   sf_realloc(b, 48)   * |  48 bytes            |-|  32 bytes   | *
| payload: 32 bytes    |                       * |                      |-|             | *
|                      |                       * |  total blocksize: 80 |-|             | *
|                      |                       * |  payload: 48 bytes   |-|             | *
+----------------------+                       * +----------------------+ +-------------+ *
                                               ********************************************
                                    *b is now a memory block of size 80 bytes, but the user
                                     can only use the requested 48 bytes.
```

## Notes on sf_free

When implementing your `sf_free` function, consider the case where an invalid pointer is passed to the function. Before freeing this pointer, your `sf_free` function should check if the pointer being freed actually belongs to an allocated block. If this pointer does not belong to an allocated block then `sf_free` should return `NULL` and set `errno` to indicate that it was passed an invalid argument.

To check if the pointer passed in to your `sf_free` function is valid, examine the header and footer of the allocated block being freed. You should verify that all fields stored in the header and footer are the same. These fields include the allocated bit, splinter bit, and requested size.

# Helper Functions

The `sfutil` library additionally contains the following helper functions:

```c
/*
 * Function which outputs the state of the free-list to stdout.
 * Performs checks on the placement of the header and footer,
 * and if the memory payload is correctly aligned.
 * See sf_snapshot section for details on output format.
 * @param verbose If true, snapshot will additionally print out
 * each memory block using sf_blockprint function.
 */
void sf_snapshot(bool verbose);

/*
* Function which prints human readable block format
* @param block Address of the block header in memory.
*/
void sf_blockprint(void *block);

/*
 * Prints human readable block format from the address of the payload.
 * IE. subtracts header size from the data pointer to obtain the address
 * of the block header. Calls sf_blockprint internally to print.
 * @param data Pointer to payload data in memory
 * (value returned by sf_malloc).
 */
void sf_varprint(void *data);
```

The functions `sf_snapshot`, `sf_blockprint`, and `sf_varprint` are helper functions which we have provided to help you visualize your freelist and allocated blocks. We have also provided you with additional unit tests which will check certain properties of each free block when a snapshot is being performed and the snapshot verbose value is set to **true**. It would be in your best interest to make sure your allocator passes all verbose snapshot tests.

## sf_blockprint and sf_varprint

These two functions print to stdout a visual representation of your memory block. The function **sf_blockprint** takes the address of a memory block header.

The function **sf_varprint** takes the address of the payload of a memory block (the address that is returned by **sf_malloc**) and prints to stdout the same visual representation of the memory block. Below we display the format printed by **sf_blockprint** and **sf_varprint**.

The block size field displays the value of that field << 4. Which is the total number of bytes of the block, the header, the footer, the payload, and the padding if there was any. The value printed is INCLUSIVE of the lower four bits.

**Allocated Block:**
>
    +-------------+--------------+---------+---------------+------------+---------+-------+
    | padding_size| splinter_size| unused  | requested_size| block_size | splinter| alloc |  <- Header Block
    |   in bytes  |    in bytes  |  9 bits |   in bytes    |   in bytes |  0s     |  0a   |
    |   4 bits    |    5 bits    |         |   14 bits     |   28 bits  | 2 bits  | 2 bits|
    +-------------+--------------+---------+---------------+------------+---------+-------+
    |                                                                                     |
    |                             Payload and Padding                                     |
    |                                (N Memory Rows)                                      |
    |                                                                                     |
    |                                                                                     |
    +------------------------------------------------------+------------+---------+-------+
    |                         unused                       | block_size | splinter| alloc |  <- Footer Block
    |                                                      |   in bytes |  0s     |  0a   |
    |                        32 bits                       |   28 bits  | 2 bits  | 2 bits|
    +------------------------------------------------------+------------+---------+-------+




**Free Block:**
>
    +-------------+--------------+---------+---------------+------------+---------+-------+
    | padding_size| splinter_size| unused  | requested_size| block_size | splinter| alloc |  <- Header Block
    |   in bytes  |    in bytes  |  9 bits |   in bytes    |   in bytes |  0s     |  0a   |
    |   4 bits    |    5 bits    |         |   14 bits     |   28 bits  | 2 bits  | 2 bits|
    +-------------+--------------+---------+---------------+------------+---------+-------+
    | Next: %p                                                                            |
    +-------------------------------------------------------------------------------------+
    | Prev: %p                                                                            |
    +------------------------------------------------------+------------+---------+-------+
    |                         unused                       | block_size | splinter| alloc |  <- Footer Block
    |                                                      |   in bytes |  0s     |  0a   |
    |                        32 bits                       |   28 bits  | 2 bits  | 2 bits|
    +------------------------------------------------------+------------+---------+-------+




## sf_snapshot

You will use this function to help debug/visualize the freelist. It starts by printing out information about the snapshot: which kind of freelist is being used (hard coded to Explicit), the size of the memory row (bytes), and the total space (bytes) taken from the heap so far. It then prints out the address of the free block and how many total bytes allocated for the entire freeblock. It will print this for each node in the free list.

```c
Snapshot: Explicit 8 1024
# 2017-02-01 18:53
0x7ffe06e2a1b0 24
```
If you call **sf_snapshot** with the **verbose** flag set to true, it will perform the following additional checks per freeblock.

```
#Examples of the types of checks that may happen
WARN: Dumping header value: 0x0000001800000018. # This always prints with verbose.
WARN: Dumping footer value: 0x0000000000000000. # This always prints with verbose.

WARN: The allocated bit on the free node footer is set.
WARN: Dumping footer value: 0x0000000000000001.
WARN: The header and footer have different block sizes.

WARN: Header block size: 24
WARN: Footer block size: 0

WARN: Header padding size: 12

WARN: The payload of the block is not aligned on an address divisible by 16.

WARN: The total size of the freeblock is < 32 bytes.
WARN: This does not meet the minimum requirements for storing
WARN: The block header and footer, and the next and free pointers.
```
If you accidentally have an allocated block in your freelist, snapshot will tell you the address of which node in the freelist which is pointing to it, and dump the contents using the **sf_blockprint** function.

```
ERROR: Snapshot encountered an allocated block in the freelist...
ERROR: The freeblock  points to this block.
+----------+----------+---------------+------------+------------+----+----+
| 0 bytes  | 0 bytes  |  unused bytes |  256 bytes |  272 bytes | 00 | 01 | <- Header 0x7FC7C6D
+----------+----------+---------------+------------+------------+----+----+
| Payload:        256 bytes                                               |
+--------------------------------------------------+------------+----+----+
|                      unused                      |  272 bytes | 00 | 01 | <- Footer 0x7FC7D7D
+--------------------------------------------------+------------+----+----+

ERROR: Aborting snapshot function.
```

If the node was the head of the freelist, the message above will be slightly altered to alert you.

```
ERROR: This node is the head of the freelist.
```

If you call snapshot and the **freelist_head** pointer is `NULL`, it will tell you that the freelist is empty.

```
WARN: The freelist has no nodes...
```

## Internal and External Fragmentation

Internal Fragmentation is the wasted space in each allocated block consisting of excess memory allocated for alignment prurposes, padding, and the prevention of splinters.

External fragmentation is the total amount of free space in the current heap, separated into multiple free blocks.

Your allocator will keep track of the following metrics relevant to internal and external fragmentation. You may declare any global variable(s) to store the following information:

* `allocatedBlocks` - This variable will keep track of the current number of allocated blocks in the heap.
* `splinterBlocks` - This variable will keep track of the current number of allocated blocks in the heap that contain extra padding due to splinters.
* `padding` - This variable will keep track of the current amount of internal fragmentation caused by alignment padding in bytes.
* `splintering` - This variable will keep track of the current amount of internal fragmentation caused by splinter padding in bytes.
* `coalesces` - This variable keeps track of how many times your allocator had to coalesce free blocks since `sf_init()` was called. Each of the 3 cases (case 2-4), as outlined in the textbook and in lecture, is considered **ONE** instance of coalescing.
* `peakMemoryUtilization` - This variable keeps track of the Peak Memory Utilization of your allocator. The formula for calculating this is outlined in the textbook and in the lecture notes. Information about this statistic can be found in **Chapter 9.9.3 Page 845**.

> You **MUST** use the `static` keyword to protect the variables used to keep track of the statistics. The `static` keyword (when used at a global scope) keeps variables local to the source file. It is good practice to do so as other .c files may manipulate variables in your .c file.

When a developer wants runtime information about your allocator, they can obtain a copy of these statistics through a function you will implement named `sf_info`. These statistics are to be updated every time you allocate or free a block using `sf_malloc`, `sf_free`, or `sf_realloc`.

We have provided you with an anonymous struct`info` in `sfmm.h`. The `sf_info` function will copy statistics from your variables into an instance of this struct. This will allow a developer to access the state of the dynamic memory allocator at the time the function is called.

```c
/**
 * Data type info for keeping track and accessing data on the memory allocator
 */
typedef struct {
    size_t allocatedBlocks;
    size_t splinterBlocks;
    size_t padding;
    size_t splintering;
    size_t coalesces;
    double peakMemoryUtilization;
} info;
```



```c
/*
 *  This function will copy the the correct values to the fields
 *  in the memory info struct.
 *
 *  @param meminfo A pointer to the memory info struct passed
 *  to the function, upon return it will containt the calculated
 *  for current fragmentation
 *
 *  @return If successful return 0, if failure return -1
 */
int sf_info(info *meminfo);
```

This function takes in a `info` pointer and stores the current metric values. Since you will be internally keeping track of all this information, this function copies the values you keep track of into the memory reference by the struct pointer.

# Things to Remember

- Do not use prologue or epilogue blocks.
- **HUGE** sized allocations:
    - <font color="red">any request larger than 4 Pages (1 Page = 4096 bytes) should return NULL and set ERRNO to ENOMEM.</font>
- Make sure that memory returned is aligned and padded correctly for the system in use (64 bit Linux Mint).
- We will not grade Valgrind, however, you are encouraged to use it.

# Unit Testing

Unit testing is a part of the software development process in which the smallest testable section of one's program (units) are tested individually to ensure they are all functioning properly. This is a very common practice in industry and is often the skill most requested by companies when hiring graduates.

We will be using a C unit testing framework called [Criterion](https://github.com/Snaipe/Criterion) in which will give you some exposure to unit testing. We have provided you with a basic set of test cases and you will have to write your own test cases.

You will use this framework alongside the provided `sf_snapshot`, `sf_blockprint`, and `sf_varprint` functions to ensure your allocator works exactly as specified.

In the `tests/tests.c` file there are 5 pre-implemented unit test examples. These are examples of how to write additional test cases with the Criterion library.

The 5 unit tests provided test the following:
- `sf_malloc`'ing an integer
- Checking free block header and footer values
- Checking the splinter size after `sf_malloc`'ing values
- Checking the next and previous pointers of a free block
- Case 1 (no coalescing) after a `sf_free` call

## Compiling and Running Unit Tests

When you compile your program with `make`, a `test_main` executable will be created in the `bin` folder alongside the `main` executable. When you run the `test_main` executable you will see information about the unit tests being run on your program.

    ./test_main

To obtain more information about each test run, you can use the verbose print option.

    ./test_main --verbose=0

## Understanding & Writing Unit Tests

The first test `Malloc_an_Integer` tests `sf_malloc`. It allocates space for an integer and assigns a value to that space. It then runs an assertion to make sure that the space returned by `sf_malloc` was properly assigned.

```c
cr_assert(*x == 4, "sf_malloc failed to give proper space for int!\n");
```

The string after the comparison only gets printed to the screen if the assertion is false. However, if there is a problem before the assertion, such as a SEGFAULT, then the unit test will print to the screen the error and continue to run the rest of the unit tests.

For this assignment <font color="red">you must write 3 additional unit tests and add them to `tests.c` below the following comment.</font>

```
//############################################
//STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
//DO NOT DELETE THESE COMMENTS
//############################################
```

> For additional information on Criterion library, take a look at the official documentaion located [here](http://criterion.readthedocs.io/en/master/)! This documentation is VERY GOOD.

# Hand-in instructions
Make sure your directory tree looks like this and that your homework compiles.

<pre>
hw3
repo/hw3
├── build
│   └── sfutil.o
├── include
│   ├── debug.h
│   └── sfmm.h
├── Makefile
├── src
│   ├── main.c
│   └── sfmm.c
└── tests
    └── tests.c
</pre>

This homework's tag is: `hw3`

<pre>
$ git submit hw3
</pre>

> :nerd: When writing your program try to comment as much as possible. Try to stay consistent with your formatting. It is much easier for your TA and the professor to help you if we can fiure out what your code does quickly.
