#include <stdlib.h>
#include <stdio.h>
#include "lib.h"

_stop()
{
	extern int _erlsym;

	_setline();
	printf("Break in %d\n", _erlsym);
	exit(0);
}
