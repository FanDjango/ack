/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef _SYS_TIMEB_H
#define _SYS_TIMEB_H

struct timeb {
	time_t	time;
	unsigned short	millitm;
	short	timezone;
	short	dstflag;
};

#endif
