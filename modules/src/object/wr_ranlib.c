/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "obj.h"

void
wr_ranlib(fd, ran, cnt1)
	struct ranlib	*ran;
	long	cnt1;
{
	struct ranlib *r;
	long cnt, val;
	char *c;

	/*
	 * We overwrite the structs in r with the bytes in c, so we
	 * don't need to allocate another buffer.
	 *
	 * put4(r->ran_off, c) can fail if r->ran_off and c overlap in
	 * memory, if this is a big-endian machine.  It tries to swap
	 * the bytes from big to little endian, but overwrites some
	 * bytes before reading them.  To prevent this, we must copy
	 * each value before we overwrite it.
	 */
	r = ran;
	c = (char *)r;
	cnt = cnt1;
	while (cnt--) {
		val = r->ran_off; put4(val, c); c += 4;
		val = r->ran_pos; put4(val, c); c += 4;
		r++;
	}
	wr_bytes(fd, (char *) ran, cnt1 * SZ_RAN);
}
