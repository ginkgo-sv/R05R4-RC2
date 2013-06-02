#!/usr/bin/perl -w
#
# Copyright (c) 2000, Ga�l Roualland <gael.roualland@iname.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#      $Id: syscalls.pl,v 1.2 2005/07/25 06:34:23 magicyang Exp $
#/

# Buils syscall.h and syscallent.h from:
# - syscalls.cat containing lines of format: syscall catmask
# - syscalls.print containing lines of format: syscall [printfunction]
#   if no printfunction is provided, sys_<call> is used.
# - syscalls.master in the FreeBSD kernel source tree (/usr/src/sys/kern)

use strict;
use POSIX;

use vars qw(%sysprint %syscat);

sub usage() {
    print STDERR 
	"usage: $0 syscalls.master [<syscalls.print>] [<syscalls.cat>]\n";
    exit 1;
}

sub readprint ($) {
    my($fprint) = @_;
    
    open (PRINT, "< $fprint") || die "can't open $fprint: $!";
    while(<PRINT>) {
	chomp;
	s/^\s*//;
	s/\s+$//;
	s/#.*$//;
	my($sys, $func) = split(/\s+/);
	if (defined($sys)) {
	    if (defined($func)) {
		$sysprint{$sys} = $func;
	    } else {
		$sysprint{$sys} = "sys_$sys";
	    }
	}
    }
    close(PRINT);
}

sub readcat ($) {
    my($fcat) = @_;
    
    open (CAT, "< $fcat") || die "can't open $fcat: $!";
    while(<CAT>) {
	chomp;
	s/^\s*//;
	s/\s+$//;
	s/#.*$//;
	my($sys, $cat) = split(/\s+/);
	$syscat{$sys} = $cat if (defined($sys) && defined($cat));
    }
    close(CAT);
}


usage if (!defined($ARGV[0]) || defined($ARGV[3]));

%sysprint = ();
readprint $ARGV[1] if defined $ARGV[1];

%syscat = ();
readcat $ARGV[2] if defined $ARGV[2];

open(MASTER, "< $ARGV[0]") || die "can't open $ARGV[0]: $!";

open(SYSCALL, "> syscall.h") || die "can't create syscall.h: $!";

print SYSCALL "/*\n * Automatically generated by $0 on " . ctime(time()) . " */\n\n";
print "/*\n * Automatically generated by $0 on " . ctime(time()) . " */\n\n";

my $sysnum = 0;

while (<MASTER>) {
    chomp;
    # join broken lines
    while (/\\$/) {
	my $line;
	s/\\$//;
	$line = <MASTER>;
	chomp($line);
	$_ = "$_$line";
    }

    if (/^(\d+)\s+(?:MPSAFE\s+)?(\w+)\s+\w+\s+\{\s*([^}]+)\s*\}([^}]*)$/) {
	my($compat, $proto, $ext, $name, $nargs, @args, $pfunc, $cat);

	next if $2 eq 'OBSOL' || $2 eq 'UNIMPL';

	$compat = $2 eq 'COMPAT' ? '?' : "";

	$proto = $3;
	$ext = $4;
	
	if ($1 > $sysnum) { # syscall gap
	    while($sysnum < $1) {
		print "  { -1,\t0,\tprintargs,\t\"SYS_$sysnum\"\t}, /* $sysnum */\n";
		$sysnum++;
	    }
	} elsif ($1 < $sysnum) {
	    warn "error in master file: syscall $1 found, expecting $sysnum.";
	}
	
	if ($proto =~ /^[^\s]+\s+([^\s]+)\s*\(([^)]*)\);/) {
	    my @args = split(/,/, $2);
	    $nargs = @args;
	    $name = $1;
	    $name = $1 if ($ext =~ /^\s*([^\s]+)\s+[^\s]+\s+[^\s]+$/);
	    if (defined($sysprint{$name})) {
		$pfunc = $sysprint{$name};
		print SYSCALL "int $pfunc();\n";
	    } else {
		$pfunc = "sys_$name";
		print SYSCALL "#define $pfunc printargs\n";
	    }
	    if (defined($syscat{$name})) {
		$cat = $syscat{$name};
	    } else {
		$cat = "0";
	    }
	    $name .= $compat;
	    print "  { $nargs,\t$cat,\t$pfunc,\t\"$name\"\t}, /* $sysnum */\n";
	    $sysnum++;
	} else {
	    warn "bad syscall specification for $sysnum: $proto";
	}
    }
}
