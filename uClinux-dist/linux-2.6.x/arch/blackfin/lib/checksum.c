/*
 * File:         arch/blackfin/lib/checksum.c
 * Based on:     none - original work
 * Author:
 * Created:
 * Description:  An implementation of the TCP/IP protocol suite for the LINUX
 *               operating system.  INET is implemented using the  BSD Socket
 *               interface as the means of communication with the user level.
 *
 * Rev:          $Id: checksum.c,v 1.6 2005/12/07 09:27:33 sonicz Exp $
 *
 * Modified:
 *               Copyright 2004-2005 Analog Devices Inc.
 *
 * Bugs:         Enter bugs at http://blackfin.uclinux.org/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.
 * If not, write to the Free Software Foundation,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <net/checksum.h>
#include <asm/checksum.h>

static unsigned short do_csum(const unsigned char *buff, int len)
{

	register unsigned long sum = 0;
	int swappem = 0;

	if (1 & (unsigned long)buff) {
		sum = *buff << 8;
		buff++;
		len--;
		++swappem;
	}

	while (len > 1) {
		sum += *(unsigned short *)buff;
		buff += 2;
		len -= 2;
	}

	if (len > 0)
		sum += *buff;

	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	if (swappem)
		sum = ((sum & 0xff00) >> 8) + ((sum & 0x00ff) << 8);

	return sum;

}

/*
 *	This is a version of ip_compute_csum() optimized for IP headers,
 *	which always checksum on 4 octet boundaries.
 */
unsigned short ip_fast_csum(unsigned char *iph, unsigned int ihl)
{
	return ~do_csum(iph, ihl * 4);
}

/*
 * computes the checksum of a memory block at buff, length len,
 * and adds in "sum" (32-bit)
 *
 * returns a 32-bit number suitable for feeding into itself
 * or csum_tcpudp_magic
 *
 * this function must be called with even lengths, except
 * for the last fragment, which may be odd
 *
 * it's best to have buff aligned on a 32-bit boundary
 */
unsigned int csum_partial(const unsigned char *buff, int len, unsigned int sum)
{

	/* printk(KERN_INFO "csum_partial(%p, %i, %i)", buff, len, sum); */

	sum += do_csum(buff, len);

	sum = (sum & 0xffff) + (sum >> 16);

	/* printk(" = %04x\n", sum); */

	return sum;
}

/*
 * this routine is used for miscellaneous IP-like checksums, mainly
 * in icmp.c
 */
unsigned short ip_compute_csum(const unsigned char *buff, int len)
{
	return ~do_csum(buff, len);
}

/*
 * copy from fs while checksumming, otherwise like csum_partial
 */

unsigned int
csum_partial_copy_from_user(const unsigned char *src, unsigned char *dst,
			    int len, int sum, int *csum_err)
{
	if (csum_err)
		*csum_err = 0;
	memcpy(dst, src, len);
	return csum_partial(dst, len, sum);
}

/*
 * copy from ds while checksumming, otherwise like csum_partial
 */

unsigned int csum_partial_copy(const char *src, char *dst, int len, int sum)
{
	memcpy(dst, src, len);
	return csum_partial(dst, len, sum);
}
