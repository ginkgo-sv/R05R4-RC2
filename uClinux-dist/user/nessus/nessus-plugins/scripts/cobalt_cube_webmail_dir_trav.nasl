#
# This script was written by Michel Arboi <arboi@alussinan.org>
#
# GPL
#
# Script audit and contributions from Carmichael Security <http://www.carmichaelsecurity.com>
#      Erik Anderson <eanders@carmichaelsecurity.com>
#      Added link to the Bugtraq message archive
#
# References:
# Date:  Thu, 05 Jul 2001 03:41:50 -0400
# From: "KF" <dotslash@snosoft.com>
# To: bugtraq@securityfocus.com, recon@snosoft.com
# Subject: Cobalt Cube Webmail directory traversal
#

if(description)
{
 script_id(11073);
 script_cve_id("CAN-2001-1408");
 script_version ("$Revision: 1.2 $");
 
 name["english"] = "readmsg.php detection";
 script_name(english:name["english"]);
 
 desc["english"] = "/base/webmail/readmsg.php was detected.
Some versions of this CGI allow remote users to read local
files with the permission of the web server.
Note that if the user has a shell access, this kind of attack is 
not interesting.

*** Nessus just checked the presence of this file 
*** but did not try to exploit the flaw.
   
Solution : get a newer software from Cobalt

Reference : http://online.securityfocus.com/archive/1/195165

Risk factor : Low";


 script_description(english:desc["english"]);
 
 summary["english"] = "Checks for the presence of Cobal Cube webmail";

 script_summary(english:summary["english"]);
 
 script_category(ACT_GATHER_INFO);
 
 
 script_copyright(english:"This script is Copyright (C) 2002 Michel Arboi");
 family["english"] = "CGI abuses";
 script_family(english:family["english"]);
 script_dependencie("find_service.nes", "no404.nasl");
 script_require_ports("Services/www", 444);
 exit(0);
}

#

port = is_cgi_installed("/base/webmail/readmsg.php");
if(port) security_warning(port);

# The attack is:
# http://YOURCOBALTBOX:444/base/webmail/readmsg.php?mailbox=../../../../../../../../../../../../../../etc/passwd&id=1
