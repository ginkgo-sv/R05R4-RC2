#
# $Id: net.nic.sh,v 1.1.1.1 2004/07/19 09:24:28 lgsoft Exp $
#
if [ -n "$UML_public_CTL" ]
then
    net_eth0="eth0=daemon,10:00:00:de:ad:ba,unix,$UML_public_CTL,$UML_public_DATA";
else
    net_eth0="eth0=mcast,10:00:00:de:ad:ba,239.192.1.2,31200";
fi

net="$net_eth0"




