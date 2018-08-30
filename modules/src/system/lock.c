/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <string.h>
#include <stdlib.h>
#include "system.h"

int
sys_lock(path)
	char *path;
{
	char buf[1024];
	char *tmpf = ".lockXXXXXX";
	char *p;
	int ok, fd;

	strcpy(buf, path);
	if (p = strrchr(buf, '/')) {
		++p;
		strcpy(p, tmpf);
	}
	else
		strcpy(buf, tmpf);
	if ((fd = mkstemp(buf)) < 0)
		return 0;
	close(fd);
	ok = (link(buf, path) == 0);
	unlink(buf);
	return ok;
}
