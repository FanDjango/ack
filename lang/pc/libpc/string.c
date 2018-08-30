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

#include "pc.h"

/* function strbuf(var b:charbuf):string; */

char* strbuf(char* s)
{
	return (s);
}

/* function strtobuf(s:string; var b:charbuf; blen:integer):integer; */

int strtobuf(char* s, char* b, int l)
{
	int i;

	i = 0;
	while (--l >= 0)
	{
		if ((*b++ = *s++) == 0)
			break;
		i++;
	}
	return (i);
}

/* function strlen(s:string):integer; */

int strlen(char* s)
{
	int i;

	i = 0;
	while (*s++)
		i++;
	return (i);
}

/* function strfetch(s:string; i:integer):char; */

int strfetch(char* s, int i)
{
	return (s[i - 1]);
}

/* procedure strstore(s:string; i:integer; c:char); */

void strstore(char* s, int i, int c)
{
	s[i - 1] = c;
}
