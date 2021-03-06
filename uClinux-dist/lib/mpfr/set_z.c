/* mpfr_set_z -- set a floating-point number from a multiple-precision integer

Copyright 1999, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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

#define MPFR_NEED_LONGLONG_H
#include "mpfr-impl.h"

/* set f to the integer z */
int 
mpfr_set_z (mpfr_ptr f, mpz_srcptr z, mp_rnd_t rnd_mode)
{
  mp_size_t fn, zn, dif;
  int k, sign_z, inex;
  mp_limb_t *fp, *zp;
  mp_exp_t exp;

  MPFR_CLEAR_FLAGS (f); /* z cannot be NaN nor Inf */

  sign_z = mpz_cmp_ui (z, 0);

  if (MPFR_UNLIKELY(sign_z == 0))
    {
      MPFR_SET_ZERO(f);
      MPFR_SET_POS(f);
      MPFR_RET(0);
    }

  fp = MPFR_MANT(f);
  fn = MPFR_LIMB_SIZE(f);
  zn = ABS(SIZ(z));
  MPFR_ASSERTD (zn >= 1);
  dif = zn - fn;
  zp = PTR(z);
  count_leading_zeros(k, zp[zn-1]);

  if (MPFR_UNLIKELY(zn > MPFR_EMAX_MAX / BITS_PER_MP_LIMB + 1))
    return mpfr_set_overflow(f, rnd_mode, sign_z);
  /* because zn >= __gmpfr_emax / BITS_PER_MP_LIMB + 2
     and zn * BITS_PER_MP_LIMB >= __gmpfr_emax + BITS_PER_MP_LIMB + 1
     and exp = zn * BITS_PER_MP_LIMB - k > __gmpfr_emax */

  /* now zn <= MPFR_EMAX_MAX / BITS_PER_MP_LIMB + 1
     thus zn * BITS_PER_MP_LIMB <= MPFR_EMAX_MAX + BITS_PER_MP_LIMB
     and exp = zn * BITS_PER_MP_LIMB - k
             <= MPFR_EMAX_MAX + BITS_PER_MP_LIMB */
  exp = (mp_prec_t) zn * BITS_PER_MP_LIMB - k;
  /* The exponent will be exp or exp + 1 (due to rounding) */
  if (MPFR_UNLIKELY(exp > __gmpfr_emax))
    return mpfr_set_overflow(f, rnd_mode, sign_z);
  if (MPFR_UNLIKELY(exp + 1 < __gmpfr_emin))
    return mpfr_set_underflow(f, rnd_mode == GMP_RNDN ? GMP_RNDZ : rnd_mode,
                              sign_z);
  if (sign_z > 0)
    MPFR_SET_POS (f);
  else 
    MPFR_SET_NEG (f);

  if (dif >= 0)
    {
      mp_limb_t cc;
      int sh, to0;

      /* number has to be truncated */
      if (MPFR_LIKELY(k != 0))
        {
          mpn_lshift(fp, zp + dif, fn, k);
          if (dif != 0)
            fp[0] += zp[dif - 1] >> (BITS_PER_MP_LIMB - k);
        }
      else
        MPN_COPY(fp, zp + dif, fn);

      sh = (mp_prec_t) fn * BITS_PER_MP_LIMB - MPFR_PREC(f);
      cc = fp[0] & MPFR_LIMB_MASK (sh);
      fp[0] &= ~cc;

      to0 = rnd_mode == GMP_RNDZ
        || (rnd_mode == GMP_RNDU && sign_z < 0)
        || (rnd_mode == GMP_RNDD && sign_z > 0);

      /* remaining bits... */
      if (MPFR_LIKELY(rnd_mode == GMP_RNDN))
        {
          /* 1) If rounding bit is 0, behave like rounding to 0.
             2) Determine the sticky bit (cc != 0). */
          if (sh != 0)
            {
              mp_limb_t rb;

              rb = MPFR_LIMB_ONE << (sh - 1);
              if ((cc & rb) == 0)
                to0 = 1; /* rounding bit is 0 */
              else
                cc &= ~rb;
              if (cc == 0 && dif > 0)
                cc = zp[--dif] << k;
            }
          else /* sh == 0 */
            {
              MPFR_ASSERTN(cc == 0);
              if (dif > 0)
                cc = zp[--dif] << k;
              if ((cc & MPFR_LIMB_HIGHBIT) == 0)
                to0 = 1; /* rounding bit is 0 */
              else
                cc <<= 1;
            }

          while (cc == 0 && dif > 0)
            cc = zp[--dif];

          if (!to0 && cc == 0) /* even rounding */
            {
              cc = 1;
              if ((fp[0] & (MPFR_LIMB_ONE << sh)) == 0)
                to0 = 1;
            }
        } /* rnd_mode == GMP_RNDN */
      else if (cc == 0 && dif > 0)
        {
          cc = zp[--dif] << k;
          while (cc == 0 && dif > 0)
            cc = zp[--dif];
        }

      if (cc == 0)
        inex = 0;
      else if (to0)
        inex = -sign_z;
      else
        {
          if (MPFR_UNLIKELY(mpn_add_1(fp, fp, fn, MPFR_LIMB_ONE << sh)))
            {
              if (MPFR_UNLIKELY(exp == __gmpfr_emax))
                return mpfr_set_overflow(f, rnd_mode, sign_z);
              else
                {
                  exp++;
                  fp[fn-1] = MPFR_LIMB_HIGHBIT;
                }
            }
          inex = sign_z;
        }
    } /* dif >= 0 */
  else /* dif < 0 */
    {
      if (MPFR_LIKELY(k != 0))
        mpn_lshift(fp - dif, zp, zn, k);
      else
        MPN_COPY(fp - dif, zp, zn);
      /* fill with zeroes */
      MPN_ZERO(fp, -dif);
      inex = 0; /* result is exact */
    }

  if (MPFR_UNLIKELY(exp < __gmpfr_emin))
    {
      if (rnd_mode == GMP_RNDN && inex == 0 && mpfr_powerof2_raw (f))
        rnd_mode = GMP_RNDZ;
      return mpfr_set_underflow(f, rnd_mode, sign_z);
    }
  MPFR_SET_EXP (f, exp);
  MPFR_RET(inex);
}
