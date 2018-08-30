/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
void cs_machinit(void *vp);	/* (FILE *f)
				 * Read phase-specific information from f.
				 */

bool may_become_aar(avail_p avp);
				/*
				 * Return whether a LAR/SAR may become
				 * an AAR LOI/STI.
				 */

bool may_become_dv(void);	/*
				 * Return whether an RMI/RMU may become
				 * a DVI/DVU: a % b = a - (a / b * b).
				 */

bool desirable(avail_p avp);	/*
				 * Return whether it is desirable to eliminate
				 * the recurrences of the expression in avp.
				 * At the same time delete the recurrences
				 * for which it is not allowed.
				 */
