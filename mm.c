/*
 This implementation uses the implicit free list without combining the adjescent free blocks of memory
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"



/*Define the global variables containing start and end point of VM got from the OS*/
//startOfChunk =nullptr ; /*Start of the memory head*/
//endOfChunk = nullptr; /*End of the memory head*/
//brek = nullptr; /*current free memlocation*/

void *startOfChunk = NULL; /*Start of the memory head*/
void *endOfChunk = NULL ; /*End of the memory head*/
void *brek = NULL   ; /*current free memlocation*/

/*
 * Choose the alignment, in bytes, that will be forced on all blocks
 * allocated.  All allocated blocks will be rounded up to a multiple
 * of this size; for that reason they will also automatically be
 * aligned on a boundary of this size.
 *
 * Alignment chosen to be 4  bytes.
 */


/*
 * The following typedef'd struct defines the header and footer that is placed
 * before and after every block of allocated memory.
 */

/*
 * The following inline function generates an expression that rounds
 * up to the next highest multiple of ALIGNMENT.  However, it only
 * works if ALIGNMENT is a power of 2.  
 *
 * If "size" is already aligned, the function is a no-op.  This fact is
 * used in somme of the other functions.
 */
static inline size_t align(size_t size)
{
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

/*
 * The following macro generates an aligned (i.e., rounded-up) version
 * of the amount of space used for the header.  It will be used in
 * some subsequent macros.
 *
 *
 
 */

/*
 * The following function converts a pointer to a block header into a
 * pointer to its payload.  To do so, it first typecasts the pointer
 * to "char *" (so that pointer arithmetic will be done on a byte
 * basis), and then casts it back to "void *" (because that's what
 * malloc will return).
 *

 *
 * 
 */
static inline void *header_to_payload(header *p)
{
    return (void *)((char *)p + HEADER_SIZE);
}

/*
 * If you write a HEADER_TO_FOOTER function, it will turn out to be a
 * Good Thing if it accepts a size argument.  Remove the #if and #endif
 * to enable this function.
 */

static inline footer *header_to_footer(header *p, size_t size)
{

    return (footer *)((char *)p + size-FOOTER_SIZE);
}


/*
 * The following function is the reverse of header_to_payload.  It takes
 * a pointer to a payload and turns it into a correctly typecast
 * pointer to the corresponding header.
 *
 * Note that there probably won't be a need for payload_to_footer, but
 * if you need it you can construct it from this function and
 * header_to_footer.
 */
static inline header *payload_to_header(void *p)
{
    return (header *)((char *)p - HEADER_SIZE);
}

/*Function to combine the size field with whether the block
is used since size is a multiple of ALIGNMENT last 2 binary digits
are always zero in size */
static inline size_t combine_size_used(size_t size, size_t used)
{
	return (size | used);

}

/*Function to return the size of allocation from the combined header using a pointer to payload*/
static inline size_t get_size(void *p)
{

	return (*(size_t *)(p) &  ~0x3);

}

/*Function to return if the allocation is used  from the combined header using a pointer to payload*/
static inline size_t get_used(void *p)
{
  //header hdr = *(size_t *)(payload_to_header(p));
	return (*(size_t *)(p) &  0x1);

}




/*
 * Actualy memory-allocation functions follow this point.
 */

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	static header hdr;
	static footer ftr;
	if (brek != NULL) {
		 static size_t prev_size;
        prev_size = (void *)endOfChunk - (void *)startOfChunk;
		startOfChunk = realloc(startOfChunk, ((void *)endOfChunk - (void *)startOfChunk + CHUNK_SIZE));
		endOfChunk = (void *)startOfChunk + CHUNK_SIZE+ prev_size;
		brek = (char *)startOfChunk + 3*HEADER_SIZE; /*set the current memory pointer*/
		return 0;

	}





		//hdr.size = combine_size_used(2*HEADER_SIZE, USED);
		//ftr.size = combine_size_used(2*HEADER_SIZE, USED);
		startOfChunk = (void *)malloc(CHUNK_SIZE);
		endOfChunk = startOfChunk + CHUNK_SIZE;
		((header *)((void *)startOfChunk))->size = 0;  /*zero padding at the start of chunk*/
		((header *)((void *)startOfChunk + HEADER_SIZE))->size = combine_size_used(2*HEADER_SIZE, USED); /*prelogue*/
		((footer *)((void *)startOfChunk + 2*HEADER_SIZE))->size = combine_size_used(2*HEADER_SIZE, USED);/*prelogue*/
		brek = (void *)startOfChunk + 2*HEADER_SIZE+FOOTER_SIZE; /*set the memory pointer*/
		//ftr.size = combine_size_used(0, USED);
		((footer *)(brek))->size = combine_size_used(0, USED); /*epilogue*/


    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brek pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{

	/*
	* The new size needs to allow for the header and footer.
	*/
	size_t newsize = align(size + HEADER_SIZE + FOOTER_SIZE);

    if (size == 0)
        return NULL;


    else
	{
	  header *hdr = (header *)(find_fit(newsize));
		if (hdr->size == 0)
			return NULL;
		else {

			return header_to_payload(hdr);
		}
	}
}
/*The finction below finds empty memory locations which fits the size requested and returns the first fit*/
void *find_fit(size_t newsize)
{
	static header hdr;
	static footer ftr;
	void *search_loc = brek;
	while(1) {
	if ((*(size_t *)(search_loc) &  ~0x3) == 0 && (*(size_t *)(search_loc) &  0x1) == 1) //if epilogue
	{
		if (search_loc+newsize > endOfChunk) { //if current unused chunk is not enough
			mm_init();

		}
		else
		{
			hdr.size =combine_size_used(newsize, USED);
			ftr.size = combine_size_used(newsize, USED);
			((header *)(search_loc))->size = combine_size_used(newsize, USED);
			((footer *)(header_to_footer(search_loc, newsize)))->size= combine_size_used(newsize, USED) ;
			ftr.size = combine_size_used(0, USED);
			((footer *)(search_loc+get_size(search_loc)))->size= combine_size_used(0, USED);
			return search_loc;
			}
	}
	else if((*(size_t *)(search_loc) &  ~0x3) != 0 && (*(size_t *)(search_loc) &  0x1) == 1)
	{
		search_loc = search_loc + get_size(search_loc);
	}
	else if((*(size_t *)(search_loc) &  ~0x3) != 0 && (*(size_t *)(search_loc) &  0x1) != 1)
	{
	  if (get_size(search_loc)>=newsize){
		  ((header *)(search_loc))->size = combine_size_used(get_size(search_loc), USED);
		((footer *)(header_to_footer(search_loc, get_size(search_loc))))->size =  combine_size_used(get_size(search_loc), USED);
		return search_loc;
	  }
		else
			search_loc = search_loc + get_size(search_loc);
	}

    }



}

/*
 * mm_free - Only changes the used flag does not combine nearby free locations.
 */
void mm_free(void *ptr)
{
	(payload_to_header((void *)ptr))->size = combine_size_used(get_size(payload_to_header((void *)ptr)), UNUSED) ;
	(header_to_footer(payload_to_header((void *)ptr), get_size(payload_to_header((void *)ptr))))->size = combine_size_used(get_size(payload_to_header((void *)ptr)), UNUSED);
}

/*
 * mm_realloc - Implemented simply and fairly stupidly in terms of mm_malloc
 * and mm_free.
 */
void *mm_realloc(void *ptr, size_t newSize)
{
    header *oldhdr = payload_to_header((void *)ptr);
    void *newPayload;
    size_t copySize;

    /*
     * Handle corner cases in the specification.
     */
    if (newSize == 0) {
        if (ptr != NULL)
            mm_free((void *)ptr);
        return NULL;
    }
    else if ((void *)ptr == NULL) {
        return mm_malloc(newSize);
    }

    /*
     * Allocate new space, and copy it over.
     */
    else if ((payload_to_header((void *)ptr))->size >=newSize+2*HEADER_SIZE)
    {
        return (void *)ptr;
    }
    else{
    newPayload = mm_malloc(newSize);
    if (newPayload == NULL)
      return NULL;
    copySize = oldhdr->size - HEADER_SIZE-FOOTER_SIZE;
    if (newSize < copySize)
        copySize = newSize;
    memcpy(newPayload, (void *)ptr, copySize);
    mm_free((void *)ptr);
    return newPayload;}
}
