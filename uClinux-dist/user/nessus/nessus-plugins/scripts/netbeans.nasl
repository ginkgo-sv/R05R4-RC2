#
# This script was written by Renaud Deraison <deraison@cvs.nessus.org>
#
# See the Nessus Scripts License for details
#

if(description)
{
 script_id(10149);
 script_version ("$Revision: 1.2 $");
 script_bugtraq_id(816);
 script_cve_id("CAN-1999-1527");
 name["english"] = "NetBeans Java IDE";
 name["francais"] = "NetBeans Java IDE";
 
 script_name(english:name["english"], francais:name["francais"]);
 
 desc["english"] = "
The remote host is running NetBeans (recently renamed to
Forte') Java IDE. There is a bug in this version that allows
anyone to browse the files on this system.


Solution : Set the HTTP server 'Enable' to FALSE in Project settings
Risk factor : High";

 desc["francais"] = "
Le syst�me distant fait tourner l'IDE Java NetBeans (renomm� r�cemment Forte')
Il y a un probl�me dans cette version qui permet � n'importe qui
de browser les fichiers pr�sents sur ce syst�me.

Solution : Mettez le 'Enable' de HTTP Server � FAUX dans les project settings
";

 script_description(english:desc["english"], francais:desc["francais"]);
 
 summary["english"] = "determines whether the remote root directory is browseable";
 summary["francais"] = "determines whether the remote root directory is browseable";
 script_summary(english:summary["english"], francais:summary["francais"]);
 
 script_category(ACT_GATHER_INFO);
 
 
 script_copyright(english:"This script is Copyright (C) 1999 Renaud Deraison",
		francais:"Ce script est Copyright (C) 1999 Renaud Deraison");
 family["english"] = "Remote file access";
 family["francais"] = "Acc�s aux fichiers distants";
 script_family(english:family["english"], francais:family["francais"]);
 script_dependencie("find_service.nes", "httpver.nasl");
 script_require_ports("Services/www", 8082);
 exit(0);
}

#
# The script code starts here
#
include("http_func.inc");

if(get_port_state(8082))
{
 port = 8082;
if(get_port_state(port))
{
  soc = http_open_socket(port);
  if(soc)
  {
  buffer = http_get(item:"/", port:port);
  send(socket:soc, data:buffer);
  data = http_recv(socket:soc);
  http_close_socket(soc);
  data_low = tolower(data);
  seek = "<title>index of /</title>";
  if(seek >< data_low)
  {
   if("netbeans" >< data_low) security_hole(port);
  }
  exit(0);
 } 
}
}
#
# NetBeans might be running on another port.
# 
port = get_kb_item("Services/www");
if(!port)exit(0);
if(!(port == 80))
{
 soc = http_open_socket(port);
 if(soc)
 {
  buffer = http_get(item:"/", port:port);
  send(socket:soc, data:buffer);
  data = http_recv(socket:soc);
  data_low = tolower(data);
  seek = "<title>index of /</title>";
  if(seek >< data_low)
  {
   if("netbeans" >< data_low) security_hole(port);
  }
  http_close_socket(soc);
 }
}
