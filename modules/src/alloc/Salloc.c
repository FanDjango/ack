/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	M E M O R Y  A L L O C A T I O N  R O U T I N E S	*/

/*	The memory allocation routines offered in this file are:

	char *Salloc(str, n)	: allocate n bytes, initialized with the string
					str

	This file imports routines from "system".
*/

#include	<system.h>
#include	"in_all.h"
#include	"alloc.h"

EXPORT char *
Salloc(str, sz)
	register char *str;
	register unsigned int sz;
{
	/*	Salloc() is not a primitive function: it just allocates a
		piece of storage and copies a given string into it.
	*/
	char *res = malloc(sz);
	register char *m = res;

	if (m == 0) No_Mem();
	while (sz--)
		*m++ = *str++;
	return res;
}
