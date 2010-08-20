/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  U S E  -  D E F I N I T I O N   A N A L Y S I S
 *
 *  U D _ D E F S . C
 */

#include <em_mnem.h>
#include "../share/types.h"
#include "ud.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/map.h"
#include "../share/locals.h"
#include "ud_defs.h"
#include "../share/alloc.h"
#include "../share/aux.h"

short nrdefs;		/* total number of definitions */
short nrexpldefs;	/* number of explicit definitions */
line_p *defs;
cset *vardefs;

STATIC cset all_globl_defs, all_indir_defs;
/* auxiliary sets, used by gen_sets */


bool does_expl_def(l)
	line_p l;
{
	/* See if instruction l does an explicit definition */

	switch(INSTR(l)) {
		case op_stl:
		case op_sdl:
		case op_ste:
		case op_sde:
		case op_inl:
		case op_del:
		case op_ine:
		case op_dee:
		case op_zrl:
		case op_zre:
			return TRUE;
		default:
			return FALSE;
	}
	/* NOTREACHED */
}



bool does_impl_def(l)
	line_p l;
{
	/* See if instruction l does an implicit definition */

	switch(INSTR(l)) {
		case op_cal:
		case op_cai:
		case op_sil:
		case op_stf:
		case op_sti:
		case op_sts:
		case op_sdf:
		case op_sar:
		case op_blm:
		case op_bls:
		case op_zrf:
			return TRUE;
		default:
			return FALSE;
	}
}


make_defs(p)
	proc_p p;
{
	/* Make a map of all explicit definitions
	 * occurring in p.
	 * Determine the set of explicit definitions
	 * of variable v (i.e. vardefs[v]), for all
	 * v from 1 to nrvars.
	 * For every basic block b, compute CHGVARS(b),
	 * i.e. the set of variables changed in b by an
	 * explicit definition.
	 */

	register bblock_p b;
	register  line_p l;
	short v, i, cnt = 0;
	bool  found;

	/* first count the number of definitions */
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (l = b->b_start; l != (line_p) 0 ; l = l->l_next) {
			if (does_expl_def(l)) {
				var_nr(l,&v,&found);
				if (!found) continue; /* no ud for this var */
				cnt++;
			}
		}
	}
	nrexpldefs = cnt;
	/* now allocate the defs table and the vardefs table*/
	defs = (line_p *) newmap(nrexpldefs);
	vardefs = (cset *) newmap(nrvars);
	for (i = 1; i <= nrvars; i++) {
		vardefs[i] = Cempty_set(nrexpldefs);
	}
	cnt = 1;
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		CHGVARS(b) =Cempty_set(nrvars);
		for (l = b->b_start; l != (line_p) 0 ; l = l->l_next) {
			if (does_expl_def(l)) {
				var_nr(l,&v,&found);
				if (!found) continue;
				assert (v <= nrvars);
				Cadd(v,&CHGVARS(b));
				defs[cnt] = l;
				Cadd(cnt,&vardefs[v]);
				cnt++;
			}
		}
	}
}



STATIC init_gen(nrdefs)
	short nrdefs;
{
	/* Initializing routine of gen_sets. Compute the set
	 * of all implicit definitions to global variables
	 * (all_globl_defs) and the set of all implicit
	 * definition generated by an indirect assignment
	 * through a pointer (all_indir_defs).
	 */

	short v;

	all_globl_defs = Cempty_set(nrdefs);
	all_indir_defs = Cempty_set(nrdefs);
	for (v = 1; v <= nrglobals; v++) {
		Cadd(IMPLICIT_DEF(GLOB_TO_VARNR(v)), &all_globl_defs);
		Cadd(IMPLICIT_DEF(GLOB_TO_VARNR(v)), &all_indir_defs);
	}
	for (v = 1; v <= nrlocals; v++) {
		if (!IS_REGVAR(locals[v])) {
			Cadd(IMPLICIT_DEF(LOC_TO_VARNR(v)), &all_indir_defs);
		}
	}
}



STATIC clean_gen()
{
	Cdeleteset(all_globl_defs);
	Cdeleteset(all_indir_defs);
}



STATIC bool same_target(l,defnr)
	line_p l;
	short  defnr;
{
	/* See if l defines the same variable as def */

	line_p def;
	short  v;

	if (IS_IMPL_DEF(defnr)) {
		/* An implicitly generated definition */
		v = IMPL_VAR(TO_IMPLICIT(defnr));
		if (IS_GLOBAL(v)) {
			return TYPE(l) == OPOBJECT &&
				OBJ(l)->o_globnr == TO_GLOBAL(v);
		} else {
			return TYPE(l) != OPOBJECT &&
				locals[TO_LOCAL(v)]->lc_off == off_set(l);
		}
	}
	/* explicit definition */
	def = defs[TO_EXPLICIT(defnr)];
	if (TYPE(l) == OPOBJECT) {
		return TYPE(def) == OPOBJECT && OBJ(def) == OBJ(l);
	} else {
		return TYPE(def) != OPOBJECT && off_set(def) == off_set(l);
	}
}



STATIC rem_prev_defs(l,gen_p)
	line_p l;
	cset   *gen_p;
{
	/* Remove all definitions in gen that define the
	 * same variable as l.
	 */

	cset gen;
	Cindex i,next;

	gen = *gen_p;
	for (i = Cfirst(gen); i != (Cindex) 0; i = next) {
		next = Cnext(i,gen);
		if (same_target(l,Celem(i))) {
			Cremove(Celem(i),gen_p);
		}
	}
}




STATIC impl_globl_defs(p,gen_p)
	proc_p p;
	cset   *gen_p;
{
	/* Add all definitions of global variables
	 * that are generated implicitly by a call
	 * to p to the set gen_p.
	 */

	Cindex i;
	short v;
	cset ext = p->p_change->c_ext;

	for (i = Cfirst(ext); i != (Cindex) 0; i = Cnext(i,ext)) {
		if (( v = omap[Celem(i)]->o_globnr) != (short) 0) {
			/* the global variable v, for which we do
			 * maintain ud-info is changed by p, so a
			 * definition of v is generated implicitly.
			 */
			Cadd(IMPLICIT_DEF(GLOB_TO_VARNR(v)),gen_p);
		}
	}
}



STATIC impl_gen_defs(l,gen_p)
	line_p l;
	cset   *gen_p;
{
	/* Add all definitions generated implicitly by instruction l
	 * to gen_p. l may be a call or some kind of indirect
	 * assignment.
	 */

	proc_p p;

	switch(INSTR(l)) {
		case op_cal:
			p = PROC(l);
			if (BODY_KNOWN(p)) {
				impl_globl_defs(p,gen_p);
				if (!CHANGE_INDIR(p)) return;
				break;
			}
			/* else fall through ... */
		case op_cai:
			/* Indirect subroutine call or call to
			 * a subroutine whose body is not available.
			 * Assume worst case; all global
			 * variables are changed and
			 * the called proc. does a store-
			 * indirect.
			 */
			Cjoin(all_globl_defs,gen_p);
			break;
		/* default: indir. assignment */
	}
	Cjoin(all_indir_defs,gen_p);
}




gen_sets(p)
	proc_p p;
{
	/* Compute for every basic block b of p the
	 * set GEN(b) of definitions in b (explicit as
	 * well as implicit) that reach the end of b.
	 */
	
	register bblock_p b;
	register line_p   l;
	short defnr = 1;

	init_gen(nrdefs);  /* compute all_globl_defs and all_indir_defs */
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		GEN(b) = Cempty_set(nrdefs);
		for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
			if (does_impl_def(l)) {
				impl_gen_defs(l,&GEN(b));
				/* add definitions implicitly
				 * generated by subroutine call
				 * or indir. pointer assignment.
				 */
			} else {
				if (does_expl_def(l)) {
					if (defnr <= nrdefs && defs[defnr] == l) {
						rem_prev_defs(l,&GEN(b));
						/* previous defs. of same var
						 * don't reach the end of b.
						 */
						Cadd(EXPL_TO_DEFNR(defnr),&GEN(b));
						defnr++;
					}
				}
			}
		}
	}
	clean_gen();  /* clean up */
}




STATIC killed_defs(v,b)
	short v;
	bblock_p b;
{
	/* Put all definitions of v occurring outside b
	 * in KILL(b). In fact, we also put explicit
	 * definitions occurring in b, but not reaching the
	 * end of b, in KILL(b). This causes no harm.
	 */

	Cindex i;
	short d;

	for (i = Cfirst(vardefs[v]); i != (Cindex) 0; i = Cnext(i,vardefs[v])) {
		d = Celem(i);  /* d is an explicit definition of v */
		if (!Cis_elem(EXPL_TO_DEFNR(d),GEN(b))) {
			Cadd(EXPL_TO_DEFNR(d),&KILL(b));
		}
	}
	/* Also add implicit definition of v to KILL(b) */
	Cadd(IMPLICIT_DEF(v),&KILL(b));
}




kill_sets(p)
	proc_p p;
{
	/* For every basic block b of p compute the set
	 * KILL(b) of definitions outside b that define
	 * variables redefined by b.
	 * KILL(b) contains explicit as well as implicit
	 * definitions.
	 */

	register bblock_p b;
	Cindex i;
	short v;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		KILL(b) = Cempty_set(nrdefs);
		for (i = Cfirst(CHGVARS(b)); i != (Cindex) 0;
						i = Cnext(i,CHGVARS(b))) {
			v = Celem(i); /* v is a variable changed in b */
			killed_defs(v,b);
		}
	}
}