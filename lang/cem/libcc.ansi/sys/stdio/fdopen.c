/*
 * fdopen - convert a (UNIX) file descriptor into a FILE pointer
 */
/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include "../stdio/loc_incl.h"

#if ACKCONF_WANT_STDIO

FILE* fdopen(int fd, const char* mode)
{
	register int i;
	FILE* stream;
	int flags = 0;

	if (fd < 0)
		return (FILE*)NULL;
	for (i = 0; __iotab[i] != 0; i++)
		if (i >= FOPEN_MAX - 1)
			return (FILE*)NULL;

	switch (*mode++)
	{
		case 'r':
			flags |= _IOREAD | _IOREADING;
			break;
		case 'a':
			flags |= _IOAPPEND;
		case 'w':
			flags |= _IOWRITE | _IOWRITING;
			break;
		default:
			return (FILE*)NULL;
	}
	while (*mode)
	{
		switch (*mode++)
		{
			case 'b':
				continue;
			case '+':
				flags |= _IOREAD | _IOWRITE;
				continue;
			/* The sequence may be followed by aditional characters */
			default:
				break;
		}
		break;
	}

	if ((stream = (FILE*)malloc(sizeof(FILE))) == NULL)
	{
		return (FILE*)NULL;
	}

	if ((flags & _IOREAD) && (flags & _IOWRITE))
		flags &= ~(_IOREADING | _IOWRITING);

	stream->_count = 0;
	stream->_fd = fd;
	stream->_flags = flags;
	stream->_buf = NULL;
	__iotab[i] = stream;
	return stream;
}

#endif
