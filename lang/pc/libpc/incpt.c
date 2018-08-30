/* $Id$ */
/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/* Author: J.W. Stevenson */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "pc.h"

void _incpt(struct file* f)
{
	if (f->flags & EOFBIT)
		_trp(EEOF);
	f->flags |= WINDOW;
	f->flags &= ~ELNBIT;
#ifdef CPM
	do
	{
#endif
		f->ptr += f->size;
		if (f->count == 0)
		{
			f->ptr = f->bufadr;
			for (;;)
			{
				f->count = read(f->ufd, f->bufadr, f->buflen);
				if (f->count < 0)
				{
					if (errno != EINTR)
						_trp(EREAD);
					continue;
				}
				break;
			}
			if (f->count == 0)
			{
				f->flags |= EOFBIT;
				*f->ptr = '\0';
				return;
			}
		}
		if ((f->count -= f->size) < 0)
			_trp(EFTRUNC);
#ifdef CPM
	} while ((f->flags & TXTBIT) && *f->ptr == '\r');
#endif
	if (f->flags & TXTBIT)
	{
		if (*f->ptr & 0200)
			_trp(EASCII);
		if (*f->ptr == '\n')
		{
			f->flags |= ELNBIT;
			*f->ptr = ' ';
		}
#ifdef CPM
		if (*f->ptr == 26)
		{
			f->flags |= EOFBIT;
			*f->ptr = 0;
		}
#endif
	}
}
