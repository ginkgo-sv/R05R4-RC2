#
# This script was written by Renaud Deraison <deraison@cvs.nessus.org>
#
# See the Nessus Scripts License for details
#

if(description)
{
 script_id(10484);
 script_version ("$Revision: 1.2 $");

 name["english"] = "Read any file thanks to ~nobody/";
 name["francais"] = "Read any file thanks to ~nobody/";
 script_name(english:name["english"], francais:name["francais"]);
 
 desc["english"] = "
It is possible to access arbitrary files on the remote
web server by appending ~nobody/ in front of their
name (as in ~nobody/etc/passwd).

This problem is due to a misconfiguration in your Apache
server that sets UserDir to ./.

Solution : Set UserDir to public_html/ or something else
Risk factor : Serious";


 desc["francais"] = "
Il est possible de lire des fichiers arbitraires sur l'hote
distant en rajoutant ~nobody devant leur nom (comme dans
~nobody/etc/passwd).

Ce probl�me est vraisemblablement du � une mauvaise configuration
d'Apache qui met UserDir � './' au lieu d'autre chose.

Solution : changez la valeur de UserDir dans le fichier de configuration
d'Apache en quelque chose d'autre (public_html par exemple)
Facteur de risque : S�rieux";


 script_description(english:desc["english"], francais:desc["francais"]);
 
 summary["english"] = "Checks for the presence of /~nobody/etc/passwd";
 summary["francais"] = "V�rifie la pr�sence de /~nobody/etc/passwd";
 
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
port = get_kb_item("Services/www");
if(!port)port = 80;
if(get_port_state(port))
{
 soc = http_open_socket(port);
 if(soc)
 {  
  req = http_get(item:"/~nobody/etc/passwd", port:port);
  send(socket:soc, data:req);
  r = http_recv(socket:soc);
  http_close_socket(soc);
  if(egrep(pattern:".*root:.*:0:[01]:.*", string:r))security_hole(port);
 }
}

