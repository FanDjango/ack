/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

#include <math.h>
#include <ack/config.h>

#if ACKCONF_WANT_FLOAT

/* $Id$ */

double
hypot(double x, double y)
{
	/*	Computes sqrt(x*x+y*y), avoiding overflow */

	if (x < 0)
		x = -x;
	if (y < 0)
		y = -y;
	if (x > y)
	{
		double t = y;
		y = x;
		x = t;
	}
	/* sqrt(x*x+y*y) = sqrt(y*y*(x*x/(y*y)+1.0)) = y*sqrt(x*x/(y*y)+1.0) */
	if (y == 0.0)
		return 0.0;
	x /= y;
	return y * sqrt(x * x + 1.0);
}

struct complex
{
	double r, i;
};

double
cabs(struct complex p_compl)
{
	return hypot(p_compl.r, p_compl.i);
}

#endif

