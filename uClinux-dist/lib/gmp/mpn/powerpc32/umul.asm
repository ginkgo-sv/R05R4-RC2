dnl PowerPC-32 umul_ppmm -- support for longlong.h

dnl Copyright 2000, 2001 Free Software Foundation, Inc.
dnl
dnl This file is part of the GNU MP Library.
dnl
dnl The GNU MP Library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public License as
dnl published by the Free Software Foundation; either version 2.1 of the
dnl License, or (at your option) any later version.
dnl
dnl The GNU MP Library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
dnl General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public License
dnl along with the GNU MP Library; see the file COPYING.LIB.  If not, write
dnl to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
dnl Boston, MA 02111-1307, USA.

include(`../config.m4')


C mp_limb_t mpn_umul_ppmm (mp_limb_t *lowptr, mp_limb_t m1, mp_limb_t m2);
C

ASM_START()
PROLOGUE(mpn_umul_ppmm)

	C r3	lowptr
	C r4	m1
	C r5	m2

	mullw	r0, r4, r5
	mulhwu	r9, r4, r5
	stw	r0, 0(r3)
	mr	r3, r9
	blr

EPILOGUE(mpn_umul_ppmm)
