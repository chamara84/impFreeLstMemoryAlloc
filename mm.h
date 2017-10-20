#include <stdio.h>
#ifndef MM_H_
#define MM_H_
extern int mm_init (void);
extern void *mm_malloc (size_t size);
extern void mm_free (void *ptr);
extern void *mm_realloc(void *ptr, size_t size);
void *find_fit(size_t newsize);

#define ALIGNMENT 4
// Initial memory allocation from OS at mm_init
#define CHUNK_SIZE 1024
//Memory location is used
#define USED 1
//Memory location is not used
#define UNUSED 0

/*
 * Students work in teams of one or two.  Teams enter their team name,
 * personal names and login IDs in a struct of this
 * type in their bits.c file.
 */
typedef struct {
    char *teamname; /* ID1+ID2 or ID1 */
    char *name1;    /* full name of first member */
    char *id1;      /* login ID of first member */
    char *name2;    /* full name of second member (if any) */
    char *id2;      /* login ID of second member */
} team_t;

extern team_t team;

extern void *startOfChunk ; /*Start of the memory head*/
extern void *endOfChunk ; /*End of the memory head*/
extern void *brek   ; /*current free memlocation*/

typedef struct {
	//char used; //indicator whether the memory block is used is in the last 2 binary digits
    size_t size;                /* Size of this block, including header */
} header;


/*footer*/
typedef struct {
	//char used; //indicator whether the memory block is used is in the last 2 binary digits
	size_t size;                /* Size of this block, including header */
} footer;


#define HEADER_SIZE sizeof(header)
#define FOOTER_SIZE sizeof(footer)



#endif
