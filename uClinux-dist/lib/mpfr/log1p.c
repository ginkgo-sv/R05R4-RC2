/* mpfr_log1p -- Compute log(1+x)

Copyright 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "mpfr-impl.h"

 /* The computation of log1p is done by
    log1p(x)=log(1+x)                      */

int
mpfr_log1p (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
  int comp, inexact = 0;

  if (MPFR_UNLIKELY( MPFR_IS_SINGULAR(x)))
    {
      if (MPFR_IS_NAN(x)) 
	{
	  MPFR_SET_NAN(y); 
	  MPFR_RET_NAN;
	}
      /* check for inf or -inf (result is not defined) */
      else if (MPFR_IS_INF(x))
	{
	  if (MPFR_IS_POS(x))
	    {
	      MPFR_SET_INF(y);
	      MPFR_SET_POS(y);
	      MPFR_RET(0);
	    }
	  else
	    {
	      MPFR_SET_NAN(y);
	      MPFR_RET_NAN;
	    }
	}
      else /* x is zero */
	{
          MPFR_ASSERTD(MPFR_IS_ZERO(x));
	  MPFR_SET_ZERO(y);   /* log1p(+/- 0) = +/- 0 */
	  MPFR_SET_SAME_SIGN(y, x);
	  MPFR_RET(0);
	}
    }
  
  comp = mpfr_cmp_si (x, -1);
  /* log1p(x) is undefined for x < -1 */
  if (MPFR_UNLIKELY(comp <= 0)) 
    {
      if (comp == 0)
	/* x=0: log1p(-1)=-inf (division by zero) */
	{
	  MPFR_SET_INF(y);
	  MPFR_SET_NEG(y);
	  MPFR_RET(0);
	}
      MPFR_SET_NAN(y);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_FLAGS(y);

  /* General case */
  {
    /* Declaration of the intermediary variable */
    mpfr_t t;

    /* Declaration of the size variable */
    mp_prec_t Nx = MPFR_PREC(x);   /* Precision of input variable */
    mp_prec_t Ny = MPFR_PREC(y);   /* Precision of input variable */
    
    mp_prec_t Nt;   /* Precision of the intermediary variable */
    long int err;  /* Precision of error */
                
    /* compute the precision of intermediary variable */
    Nt = MAX(Nx,Ny);
    /* the optimal number of bits : see algorithms.ps */
    Nt = Nt + 5 + __gmpfr_ceil_log2 (Nt);

    /* initialise of intermediary variable */
    mpfr_init (t);

    /* First computation of cosh */
    do
      {
        /* reactualisation of the precision */
        mpfr_set_prec (t, Nt);
      
        /* compute log1p */
        mpfr_add_ui (t, x, 1, GMP_RNDN);   /* 1+x */
        mpfr_log (t, t, GMP_RNDN);        /* log(1+x)*/

        /* estimation of the error */
        /*err=Nt-(__gmpfr_ceil_log2(1+pow(2,1-MPFR_GET_EXP(t))));*/
        err = Nt - (MAX (1 - MPFR_GET_EXP (t), 0) + 1);

        /* actualisation of the precision */
        Nt += 10;
      }
    while ((err < 0) || !mpfr_can_round (t, err, GMP_RNDN, GMP_RNDZ,
                                         Ny + (rnd_mode == GMP_RNDN)));

    inexact = mpfr_set (y, t, rnd_mode);

    mpfr_clear (t);
  }

  return inexact;
}
