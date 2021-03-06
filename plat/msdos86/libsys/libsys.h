/* $Source$
 * $State$
 * $Revision$
 */

#ifndef LIBSYS_H
#define LIBSYS_H

#include <limits.h>
#include <stdint.h>
#include <sys/types.h>

#define OUT_OF_MEMORY ((void *)-1)	/* sbrk returns this on failure */

/*
 * Data structure for remembering whether each file descriptor is open in
 * text mode (O_TEXT) or binary mode (O_BINARY).
 *
 * Currently this is just a simple linked list, where each linked list node
 * records the modes for 16 file descriptors, in bit vector form (0: text,
 * 1: binary).  In addition, each node also remembers, for each of its file
 * descriptors,  whether an end-of-file (^Z) character was encountered when
 * reading from it in text mode.
 *
 * List nodes are allocated using sbrk() and never freed.
 *
 * This scheme should be fast and light enough, as the number of open file
 * descriptors is expected to be small.
 *
 * FIXME: the code for updating this structure is not exactly thread-safe.
 */
typedef uint16_t _fdvec_t;

struct _fdmodes {
	struct _fdmodes *next;
	int begfd;
	_fdvec_t modevec, eofvec;
};

extern struct _fdmodes _sys_fdmodes;

#define _FDVECMASK (sizeof(_fdvec_t) * CHAR_BIT - 1)

/* Structures for getting MS-DOS's idea of the current date and time. */

struct _dosdate {
	uint8_t day;
	uint8_t month;
	uint16_t year;
};

struct _dostime {
	uint8_t minute;
	uint8_t hour;
	uint8_t hsecond;
	uint8_t second;
};

extern int _sys_getmode(int);
extern int _sys_setmode(int, int);
extern int _sys_iseof(int);
extern int _sys_seteof(int, int);
extern ssize_t _sys_rawread(int, char *, size_t);
extern ssize_t _sys_rawwrite(int, const char *, size_t);
extern int _sys_isopen(int);
extern int _sys_isreadyr(int);
extern int _sys_exists(const char *);
extern int _sys_rawcreat(const char *, unsigned);
extern int _sys_rawopen(const char *, int);
extern off_t _sys_rawlseek(int, off_t, int);
extern void _sys_getdate(struct _dosdate *);
extern void _sys_gettime(struct _dostime *);

#endif
