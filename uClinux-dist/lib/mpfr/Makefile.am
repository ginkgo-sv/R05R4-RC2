AUTOMAKE_OPTIONS = gnu

SUBDIRS = tests
EXTRA_DIST = BUGS FAQ.html VERSION generic.c round_raw_generic.c

include_HEADERS = mpfr.h mpf2mpfr.h

lib_LIBRARIES = libmpfr.a

libmpfr_a_SOURCES = mpfr.h mpf2mpfr.h mpfr-gmp.h mpfr-impl.h mpfr-longlong.h mpfr-test.h log_b2.h exceptions.c save_expo.c extract.c uceil_exp2.c uceil_log2.c ufloor_log2.c add.c add1.c add_one_ulp.c add_ui.c agm.c clear.c cmp.c cmp_abs.c cmp_si.c cmp_ui.c comparisons.c div_2exp.c div_2si.c div_2ui.c div.c div_ui.c dump.c eq.c exp10.c exp2.c exp3.c exp.c frac.c get_d.c get_exp.c get_str.c init.c inp_str.c isinteger.c isinf.c isnan.c isnum.c const_log2.c log.c mul_2exp.c mul_2si.c mul_2ui.c mul.c mul_ui.c neg.c next.c out_str.c const_pi.c pow.c pow_si.c pow_ui.c print_raw.c print_rnd_mode.c random2.c random.c reldiff.c round_prec.c set.c setmax.c setmin.c set_d.c set_dfl_prec.c set_exp.c set_rnd.c set_f.c set_prc_raw.c set_prec.c set_q.c set_si.c set_str.c set_str_raw.c set_ui.c set_z.c sqrt.c sqrt_ui.c sub.c sub1.c sub_one_ulp.c sub_ui.c rint.c ui_div.c ui_sub.c urandomb.c get_z_exp.c swap.c  factorial.c cosh.c sinh.c tanh.c acosh.c asinh.c atanh.c atan.c cmp2.c exp_2.c asin.c const_euler.c cos.c sin.c tan.c fma.c hypot.c log1p.c expm1.c log2.c log10.c ui_pow.c ui_pow_ui.c minmax.c dim.c copysign.c gmp_op.c init2.c acos.c sin_cos.c set_nan.c set_inf.c powerof2.c gamma.c set_ld.c get_ld.c cbrt.c volatile.c fits_s.h fits_sshort.c fits_sint.c fits_slong.c fits_u.h fits_ushort.c fits_uint.c fits_ulong.c fits_uintmax.c fits_intmax.c get_si.c get_ui.c zeta.c cmp_d.c erf.c inits.c inits2.c clears.c sgn.c check.c sub1sp.c version.c mpn_exp.c mpfr-gmp.c mp_clz_tab.c sum.c add1sp.c free_cache.c si_op.c cmp_ld.c set_ui_2exp.c set_si_2exp.c set_uj.c set_sj.c get_sj.c get_uj.c get_z.c iszero.c cache.c sqr.c int_ceil_log2.c isqrt.c strtofr.c

libmpfr_a_LIBADD = @LIBOBJS@

info_TEXINFOS = mpfr.texi

mpfr_TEXINFOS = fdl.texi

MAKEINFOFLAGS = --enable-encoding
