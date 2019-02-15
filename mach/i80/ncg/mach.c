/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * machine dependent back end routines for the Intel 8080.
 */

#include <stdlib.h> /* atol */

void con_part(int sz, word w)
{

	while (part_size % sz)
		part_size++;
	if (part_size == 2)
		part_flush();
	if (sz == 1)
	{
		w &= 0xFF;
		if (part_size)
			w <<= 8;
		part_word |= w;
	}
	else
	{
		assert(sz == 2);
		part_word = w;
	}
	part_size += sz;
}

void con_mult(sz) word sz;
{

	if (argval != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile, ".data4\t%ld\n", atol(str));
}

#define CODE_GENERATOR
#define IEEEFLOAT
#define FL_MSL_AT_LOW_ADDRESS 0
#define FL_MSW_AT_LOW_ADDRESS 0
#define FL_MSB_AT_LOW_ADDRESS 0
#include <con_float>

void prolog(full nlocals)
{
	int16_t adjustment = -nlocals;

	if (adjustment == 0)
		fprintf(codefile, "\tcall .pro0\n");
	else if (adjustment == -2)
		fprintf(codefile, "\tcall .pro2\n");
	else if (adjustment == -4)
		fprintf(codefile, "\tcall .pro4\n");
	else if ((adjustment >= -128) && (adjustment <= 127))
		fprintf(codefile, "\tcall .probyte\n\t.data1 %d\n", adjustment);
	else
		fprintf(codefile, "\tcall .proword\n\t.data2 %d\n", adjustment);
}

void mes(type) word type;
{
	int argt;

	switch ((int)type)
	{
		case ms_ext:
			for (;;)
			{
				switch (argt = getarg(ptyp(sp_cend) | ptyp(sp_pnam) | sym_ptyp))
				{
					case sp_cend:
						return;
					default:
						strarg(argt);
						fprintf(codefile, ".define %s\n", argstr);
						break;
				}
			}
		default:
			while (getarg(any_ptyp) != sp_cend)
				;
			break;
	}
}

char* segname[] = { ".sect .text", ".sect .data", ".sect .rom", ".sect .bss" };
