/*
 * vprintf - formatted output without ellipsis to the standard output stream
 */
/* $Id$ */

#include <stdio.h>
#include <stdarg.h>
#include "loc_incl.h"

#if ACKCONF_WANT_STDIO

int vprintf(const char* format, va_list arg)
{
	return _doprnt(format, arg, stdout);
}

#endif
