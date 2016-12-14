/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "meganTeam",
    /* First member's full name */
    "Megan Moore",
    /* First member's email address */
    "mm7148@nyu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define WORD_SIZE 4
#define DOUBLE_SIZE 8

#define INIT_SIZE (1<<6)
#define BLOCK_SIZE (1<<12)
#define REALLOC_BUFFER (1<<7)
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define LIMIT 20//the initializing size for the list

#define MIN_BLOCK_SIZE 8//&????
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))

/*fitting a the size and allocated bit into a word block*/
#define BLOCK(size, alloc) ((ALIGN(size)) | (alloc))//CHECK????????

/*Read and write  word at address a to keep track of which lists are free or not*/
#define GET(a)          (*(unsigned int *)(a))
#define PUT(a,val)      (*(unsigned int *)(a)=(val) | GET_TAG(a))
#define PUT_NOTAG(a,val)(*(unsigned int *)(a)=(val))

/*for free blocks record previous and next pointers*/
#define SET_PTR(p,ptr)(*(unsigned int *)(a)=(unsigned int)(ptr))

/*get the size and already allocated blocks from a*/
#define GET_SIZE(a)     (GET(a) & ~0x7)
#define GET_ALLOC(a)    (GET(a) & 0x1)
#define GET_TAG(a)	(GET(a) @ 0x2)
#define SET_TAG(a)	(GET(a) |= 0x2)
#define REMOVE_TAG(a) 	(GET(a) &= ~0x2)

/*compute the address of a header and footer for a given block pointer*/
#define HEADER(ptr)   	((char *)(ptr)-WORD_SIZE)
#define FOOTER(ptr) 	((char*)(ptr)+GET_SIZE(HEADER(ptr))-DOUBLE_SIZE)
#define DATA(ptr) 	((char*)(ptr)+WORD_SIZE)

#define NEXT_HEADER(ptr)	 ((char *)(ptr) + GET_SIZE((char *)(ptr)-WORD_SIZE)
#define PREVIOUS_HEADER(ptr)	 ((char *)(ptr) - GET_SIZE((char *)(ptr)-DOUBLE_SIZE)
#define PREVIOUS_POINTER(ptr)	 ((char *)(ptr))
#define PREVIOUS(ptr)		(*(char **)(ptr))
#define NEXT(ptr)		(*(char **)(DATA(ptr)))

unsigned int *start_list;//FJAKSL;
unsigned int *end_list;//FJADKSLF;A

void *segregated_free_lists[LIMIT];

void *prologue;//NJKALSHFDAS
int name=1 ;//AHFJDASKFDS

static void *extend(size_t size)
{
    void *ptr;

    size_t newSize=ALIGN(size);
    if((ptr=mem_sbrk(newSize)) == (void*)-1)
    {
	return NULL;
    }
    PUT_NOTAG(HEADER(ptr), BLOCK(newSize,0));
    PUT_NOTAG(FOOTER(ptr), BLOCK(newSize,0));
    PUT_NOTAG(HEADER(NEXT_HEADER(ptr)),BLOCK(0,1));
    insert(ptr,newSize);

   return coalesce(ptr);
}

void reset(void *header)
{
    if(header)
    {
	PREVIOUS_POINTER(header) = NULL; 
	NEXT_POINTER(header) = NULL;
    }
}
/*checks header to determine if it will fit*/
void *find_fit(size_t size, void *header)
{
    if(!header) return NULL;
    if(size <= GET_SIZE(header))
    {
	 return header;//block fits
    }
    return find_fit(size, NEXT_POINTER(header)));
}
/*finds the best block from the beginning of the free list*/
void *find(size_t size)
{
    return find_fit(size, start_list);

}

void remove_free(void *header)
{
    if(!header) return;
    //rearrange free lists
    if(start_list == end_list)
    {
	start_list = end_list = NULL;//list is empty so both are null
    else if(start_list == header) //the first item in the free  list
    {
	start_list = NEXT_POINTER(header);
	PREV_POINTER(NEXT_POINTER(header)) = NULL;
    }
    else if (end_list == header) //found the last item on the free  list
    {
	list_end = PREV_POINTER(header);
	NEXT_POINTER(PREV_POINTER(header)) = NULL;
    }

    else
    {
	if(name)
	{
            printf("header:; %x size: %d prior pointer: %x next pointer: %x\n",(unsigned int) header GET_SIZE(header), NEXT_POINTER(header));
	}
	NEXT_POINTER(PREVIOUS_POINTER(header))=NEXT_POINTER(header);
	PREVIOUS_POINTER(NEXT_POINTER(header))=PREVIOUS_POINTER(header);
    }
}

void add(void *header)
{
	if(!header)
	{
	    return;
	}
	reset(header);
	if(!start_lsit && end_list)//list is empty
	{
            start_list = end_list = header;
	    return;
	}
	if(name)
	{
	    printf("List is not empty\n");
	}
	if(GET_SIZE(header) <= GET_SIZE(start_list))//header is smaller than anything
	{
	    NEXT_POINTER(header) = start_list;
	    PREVIOUS_POINTER(start_list) = header;
	    start_list = header;
	    return;
	}
	if(GET_SIZE(header) >= GET_SIZE(end_list))//header is larger than anything
	{
	    NEXT_POINTER(end_list)=header;
	    PREVIOUS_POINTER(header)=end_list;
	    end_list=header;
	    return;
	}
	//fits so we can add
	void *current = find(GET_SIZE(header));
	PREVIOUS_POINTER(header) = PREVIOUS_POINTER(current);
	NEXT_POINTER(PREVIOUS_POINTER(header)) = header;
	PREVIOUS_POINTER(current) = header;
	NEXT_POINTER(header) = current;
}
//checks if the memory fits within the bounds of memheaplo and memheaphi
int inbounds(void *ptr)
{
    return (ptr >= mem_heap_lo() && ptr < mem_heap_hi());
}

/*mm_coalesce checks if two adjacent blocks are free, if they are it combines them and returns the
*header for the new coalesced block.
*/
mm_coalesce(void *ptr)
{
    if(name)
    {
	printf("Enter coalesce\n");
    }
    void *header = HEADER(ptr);

    void *next = NEXT_HEADER(ptr);
    void *last = PREVIOUS_HEADER(ptr));

    if(name)
    {
 	printf("header: %x last: %x next: %x\n", header, last, next);
    }
    size_t oldSize = GET_SIZE(header);
    if(inbounds(next) && !GET_ALLOC(next);
    {
	if(name)
	{
	    printf("right coalesce\n");
        }
	remove_free(next);
        reset(next);
	GET(FOOTER(DATA(header))) = NULL;
	PUT(header, BLOCK(GET_SIZE(header) + GET_SIZE(next) + MIN_BLOCK_SIZE,0));
	GET(FOOTER(DATA(header)))=header;
    }
    if(inbounds(last) && !GET_ALLOC(last))
    {
	if(name)
	{
	    printf("left coalesce\n");
	}
	remove(last);
	reset(last);
	GET(FOOTER(DATA(last)))=NULL;
	PUT(last,BLOCK(GET_SIZE(last)+GET_SIZE(header)+ MIN_BLOCK_SIZE,0));
	GET(FOOTER(DATA(last)))=last;
	header=last;
    }
    size_t newSize = GET_SIZE(header):
    if(name && oldSize !+ newSize)
    {
	printf("oldSize: %d newSize: %d\n", oldSize, newSize);
    }
    if(name)
    {
	printf("exit coallesce");
    }
    return header;
}




/* 
 * mm_init - initialize the malloc package.
 * initialize thie initial heap area of size 48bytes with
 * segregated free lists of block sizes (128, 256, 512, 1024, 2048, 4096, 8192, 16384, 
 * 32768, 65536, 614784)
 * return -1 if problems initializing otherwise return 0
 */
int mm_init(void)
{
    if(name)
    {
	printf("Initialize memory management\n");
    }
    prologue = mem_sbrk(WORD_SIZE);
    if((int)prologue == -1) 
    {
	return -1;
    }
    PUT(prologue, BLOCK(WORD_SIZE, 1));
    start_list=NULL;
    end_list=NULL:

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 * return a pointer to an allocated block
 * return 8-byte alligned pointers
 */
void *mm_malloc(size_t size)
{
    if(name)
	printf("Enter malloc - size: %d\n", size);

    if(size <= 0) return NULL; //cannot have memory less than zero
    size=ALIGN(size);
    size_t newsize= ALIGN(size+ALIGNMENT);

    if(name && start_list && end_list)
	printf("start_list: %x size: %d: end_list: %x size: %d\n",(unsigned int)start_list, GET_SIZE(start_list), (unsigned int)end_list, GET_SIZE(end_list));
    if((start_list && (size == GET_SIZE(start_lsit) || size + MIN_BLOCK_SIZE < GET_SIZE(start_list)) || end_list && (size == GET_SIZE(end_list) || size + MIN_BLOCK_SIZE < GET_SIZE(end_list))))
    {
	if(name)
        {
	     printf("size + blocksize: %d\n", size + MIN_BLOCK_SIZE);
	}
	void *currentHeader;
	if(size == GET_SIZE(start_list))
	{
	     currentHeader=start_list;
	}
	else if(size == GET_SIZE(end_list))
	{
	    currentHeader=end_list;
	}
	else
	{
	    currentHeader= find(size + MIN_BLOCK_SIZE + MIN_BLOCK_SIZE);
	}
	if(name)
	{
	    printf("Found block: %x size: %d\n",(unsigned int)currentHeader, GET_SIZE(currentHeader));
	}
	remove_free(currentHeader);//remove it from the free list

	if(name)
	{
	    printf("Removed: %x size: %d\n", (unsigned int)currentHeader, GET_SIZE(currentHeader));
	}

	size_t oldSize=GET_SIZE(currentHeader);
	
	PUT(currentHeader, BLOCK(size,1));//set as allocated and set footer pointer
	GET(FOOTER(DATA(currentHeader))) = currentHeader;

	if(size + MIN_BLOCK_SIZE <= oldSize)
	{
	    void *nextHeader = NEXT_HEADER(DATA(currentHeader));//header of block we are making
	    PUT(nextHeader, BLOCK(oldSize-size-MIN_BLOCK_SIZE, 0));//mark as a block with header and footer
	    GET(FOOTER(DATA(nextHeader))) = nextHeader;

	    add(nextHeader)://add to free list
	}
	mm_check();
	return DATA(currentHeader);
    }

    if(name)
    {
	printf("need more heap memory\n");
    }
    void *currentHeader=mem_sbrk(newsize);//request more memory

    if((int)currentHeader == -1) return NULL; //failed

    PUT(currentHeader, BLOCK(size,1));

    if(name)
    {
	printf("Got memory: %x size: %d\n",(unsigned int) currentHeader, GET_SIZE(currentHeader));
    }
    GET(FOOTER(DATA(currentHeader))) = currentHeader;
    
    if(name)
    {
	printf("exit malloc\n");
    }
    mm_check();
    return DATA(currentHeader);//return current allocated mem block
}

/*
 * mm_free - Freeing a block does nothing.
 * frees the block pointed to by ptr. returns nothing
 * only guaranteed to work when the ptr was returned by an earlier call to mm_malloc
 * or mm_realloc and has not yet been freed
 * (check if returned earlier and if not free)
 */
void mm_free(void *ptr)
{
    if(name)
    {
	printf("Enter Free- ptr: %x size: %d\n",(unsigned int)HEADER(ptr), GET_SIZE(HEADER(ptr)));
    }
    void *header=HEADER(ptr);//get the right header
    
    if(header==NULL || !inbounds(header) || GET_ALLOC(header)==0)//pointer invalid
    {
	return;
    }

    PUT(header,BLOCK(GET_SIZE(header),0));//mark as free
    header=mm_coalesce(ptr);//coalesce if adjacent blocks are free
    add(header);//add to free list
    
    mm_check();
    
    if(name)
    {
	printf("exit free\n");
    }
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 * returns a pointer to an allocated region of at lease size bytes
 * 1)if ptr is null, the call is equivalent to mm_malloc(size)
 * 2)if size is equal to zero, the call is equivalent to mm_free(ptr)
 * 3)if ptr not null then it means it was previously returned by malloc or realloc
 * change the size of the memory block pointed to by ptr to size bytes
 * returns the address ofo the new block(may be the same as the old block)
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *header=HEADER(ptr);
    size_t newSize = ALIGN(size);
    size_t oldSize = GET_SIZE(header);

    if(!inbounds(ptr))
    {
	return mm_malloc(size);
    }
    if(size<=0)
    {
	mm_free(ptr);
	return NULL;
    }
    if(name)
    {
	printf("realloc- header: %x oldSizee: %d newSize: %d Aligned size: %d\n", (unsigned int) HEADER(ptr), GET_SIZE(HEADER(ptr)), size, newSize);
    }
    if(newSize==GET_SIZE(header))
    {
	return ptr;
    }
    if(newSize + MIN_BLOCK_SIZE < GET_SIZE(header))
    {
	PUT(header, BLOCK(newSize,1));
	GET(FOOTER(DATA(header))) = header;
	
	unsigned int *newHeader = NEXT_HEADER(DATA(header));

	printf("newHeader: %x\n", (unsigned int) newHeader);

	PUT(newHeader, BLOCK((oldSize-newSize-MIN_BLOCK_SIZE),0));
	GET(FOOTER(DATA(newHeader))) = newHeader;
	add(newHeader);
	return DATA(header);
    }
    void *newHeader==mm_coalesce(ptr);//coallesce free blocks
    if(oldSize < GET_SIZE(newHeader) && newSize + MIN_BLOCK_SIZE< GET_SIZE(newHeader));
    {
	PUT(newHeader, BLOCK(newSize,1));
	if(newHeader != header)
	{
	    memcpy(DATA(newHeader), DATA(header),size);
	}
	PUT(NEXT_HEADER(DATA(newHeader)), BLOCK(GET_SIZE(newHeader)-newSize,0));
	add(NEXT_HEADER(DATA(newHeader)));
	return DATA(newHeader);
    }
    newHeader = HEADER(mm_malloc(newSize));
    memcpy(DATA(newHeader), DATA(header), newSize);
    PUT(header, BLOCK(GET_SIZE(header),0));
    add(header);

    if(name)
    {
	printf("exit realloc- newHeader %x size: %d\n", (unsigned int) newHeader,GET_SIZE(newHeader));
    }
    return DATA(newHeader):
}

/* int mm_check(void)- checks for inconsistencies and invariants that are prudent
*  returns a nonzero value if the heap is consistent
* is every block in the free list marked as free? are there non-coallesced blocks
* is every free block in the free list
* do the pointers for the free list point to actual free blocks
* do allocated blocks overlap
* do the pointers in a heap block point to valid addresses
* prints error messages if mm_check fails
* do not call in submission!
*/
int mm_check(void)
{
    void *current=start_list;
    while(current != NULL)
    {
	if(NEXT_POINTER(current) && GET_ALLOC(NEXT_POINTER(current)))//if current block points to allocated block
	{
	    printf("Block: %x points the the allocated block\n", (unsigned int) current);
	    return 0;
	}
	if(GET_ALLOC(current))
	{
	    printf("Block: %x is in free list but marked allocated\n", (unsigned int)current);
	    return 0;
	}

	current = NEXT_POINTER(current);
    }
    current=prologue+WORD_SIZE;//beginning at the start of the stack
    void* last=NULL;//previous block
    while(inbounds(current))
    {
	if(last && current < last)//invalid size
	{
	    printf("block: %x has invalid size: %d\n", (unsigned int)previous, GET_SIZE(previous));
	    return 0;
	}
	if(last&&(!GET_ALLOC(last) && !GET_ALLOC(current)))//two free blocks next to each other
	{
	    printf("Blocks: %x and %x escaped coalescing\n", (unsigned int)current, (unsigned int) previous);
	    return 0;
	}
	previous=current;
	current=NEXT_HEADER(DATA(current));
    }
    return 1;//heap is clear
}












