/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include "libsys.h"

ssize_t read(int fd, void* buffer, size_t count)
{
	char i;
	
	/* We're only allowed to read from fd 0, 1 or 2. */
	
	if ((fd < 0) || (fd > 2))
	{
		errno = EBADF;
		return -1;
	}
	
	/* Empty buffer? */
	
	if (count == 0)
		return 0;
	
	/* Read one byte. */
	
	i = _sys_rawread();
	if ((i == '\r') && !(_sys_ttyflags & INLCR))
		i = '\n';
	if (_sys_ttyflags & ECHO)
		_sys_write_tty(i);

	*(char*)buffer = i;
	return 1;
}
