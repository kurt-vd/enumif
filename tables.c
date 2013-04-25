#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_arp.h>

#include "libenumif.h"

struct lookup {
	int i;
	const char *a;
};

static const char *lookup_i(int i, const struct lookup *table)
{
	for (; table->a; ++table) {
		if (table->i == i)
			return table->a;
	}
	return NULL;
}

static int lookup_a(const char *str, const struct lookup *table, int _default)
{
	const struct lookup *saved = NULL;

	if (*str == '#')
		return strtoul(str+1, 0, 0);

	for (; table->a; ++table) {
		if (!strncasecmp(table->a, str, strlen(str))) {
			if (saved)
				/* duplicate match */
				return _default;
			saved = table;
		}
	}
	return saved ? saved->i : _default;
}

#define __PF(f,n) { ARPHRD_##f, #n },
static const struct lookup iftypes[] = {
	{ 0, "generic", },
__PF(ETHER,ether)
__PF(EETHER,eether)
__PF(AX25,ax25)
__PF(PRONET,pronet)
__PF(CHAOS,chaos)
__PF(IEEE802,ieee802)
__PF(ARCNET,arcnet)
__PF(APPLETLK,atalk)
__PF(DLCI,dlci)
__PF(ATM,atm)
__PF(METRICOM,metricom)
__PF(IEEE1394,ieee1394)
__PF(INFINIBAND,infiniband)
__PF(SLIP,slip)
__PF(CSLIP,cslip)
__PF(SLIP6,slip6)
__PF(CSLIP6,cslip6)
__PF(RSRVD,rsrvd)
__PF(ADAPT,adapt)
__PF(ROSE,rose)
__PF(X25,x25)
__PF(HWX25,hwx25)
__PF(CAN,can)
__PF(PPP,ppp)
__PF(HDLC,hdlc)
__PF(LAPB,lapb)
__PF(DDCMP,ddcmp)
__PF(RAWHDLC,rawhdlc)
__PF(TUNNEL,ipip)
__PF(TUNNEL6,tunnel6)
__PF(FRAD,frad)
__PF(SKIP,skip)
__PF(LOOPBACK,loopback)
__PF(LOCALTLK,ltalk)
__PF(FDDI,fddi)
__PF(BIF,bif)
__PF(SIT,sit)
__PF(IPDDP,ip/ddp)
__PF(IPGRE,gre)
__PF(PIMREG,pimreg)
__PF(HIPPI,hippi)
__PF(ASH,ash)
__PF(ECONET,econet)
__PF(IRDA,irda)
__PF(FCPP,fcpp)
__PF(FCAL,fcal)
__PF(FCPL,fcpl)
__PF(FCFABRIC,fcfb0)
__PF(FCFABRIC+1,fcfb1)
__PF(FCFABRIC+2,fcfb2)
__PF(FCFABRIC+3,fcfb3)
__PF(FCFABRIC+4,fcfb4)
__PF(FCFABRIC+5,fcfb5)
__PF(FCFABRIC+6,fcfb6)
__PF(FCFABRIC+7,fcfb7)
__PF(FCFABRIC+8,fcfb8)
__PF(FCFABRIC+9,fcfb9)
__PF(FCFABRIC+10,fcfb10)
__PF(FCFABRIC+11,fcfb11)
__PF(FCFABRIC+12,fcfb12)
__PF(IEEE802_TR,tr)
__PF(IEEE80211,ieee802.11)
__PF(IEEE80211_PRISM,ieee802.11/prism)
__PF(IEEE80211_RADIOTAP,ieee802.11/radiotap)
#ifdef ARPHD_IEEE802154
__PF(IEEE802154, ieee802.15.4)
#endif
#ifdef ARPHD_PHONET
__PF(PHONET, phonet)
#endif
#ifdef ARPHD_PIPE
__PF(PHONET_PIPE, phonet_pipe)
#endif

__PF(NONE, none)
__PF(VOID,void)
	{ },
};

#define __IF(f)	{ IFF_##f, #f, }
static const struct lookup ifflags[] = {
	__IF(LOOPBACK),
	__IF(BROADCAST),
	__IF(POINTOPOINT),
	__IF(MULTICAST),
	__IF(NOARP),
	__IF(ALLMULTI),
	__IF(PROMISC),
	__IF(MASTER),
	__IF(SLAVE),
	__IF(DEBUG),
	__IF(DYNAMIC),
	__IF(AUTOMEDIA),
	__IF(PORTSEL),
	__IF(NOTRAILERS),
	__IF(UP),
	__IF(LOWER_UP),
	__IF(DORMANT),
	__IF(ECHO),
	{},
};

#define __ST(f) { IF_OPER_##f, #f, }
static const struct lookup ifstates[] = {
	__ST(UNKNOWN),
	__ST(NOTPRESENT),
	__ST(DOWN),
	__ST(LOWERLAYERDOWN),
	__ST(TESTING),
	__ST(DORMANT),
	__ST(UP),
	{},
};

const char *if_typestr(int iftype)
{
	static char buf[16];

	return lookup_i(iftype, iftypes) ?: (sprintf(buf, "#%i", iftype), buf);
}

const char *if_flagsstr(int if_flags)
{
	static char buf[1024];
	char *str = buf;
	const struct lookup *lp;

	/* from iproute2 */
	if ((if_flags & IFF_UP) && !(if_flags & IFF_RUNNING))
		str += sprintf(str, "%s%s", "NO-CARRIER", if_flags ? "," : "");
	if_flags &= ~IFF_RUNNING;
        
	for (lp = ifflags; lp->a; ++lp) {
		if (if_flags & lp->i) {
			if_flags &= ~lp->i;
			str += sprintf(str, "%s%s", lp->a, if_flags ? "," : "");
		}
	}
	if (if_flags)
		str += sprintf(str, "%x", if_flags);
	return buf;
}

const char *if_statestr(int ifstate)
{
	static char buf[16];

	return lookup_i(ifstate, ifstates) ?: (sprintf(buf, "#%i", ifstate), buf);
}

int if_strtype(const char *str)
{
	return lookup_a(str, iftypes, 0);
}

int if_strflags(const char *str)
{
	return lookup_a(str, ifflags, 0);
}

int if_strstate(const char *str)
{
	return lookup_a(str, ifstates, 0);
}
