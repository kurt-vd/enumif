/*
 * Copyright 2013 Kurt Van Dijck <kurt@vandijck-laurijssen.be>
 *
 * This file is part of libenumif.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * The netlink access code for libenumif
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>

#include "libenumif.h"

static struct {
	int sock;
	int seq;
} s = {
	.sock = -1,
};

__attribute__((destructor))
void enumif_cleanup(void)
{
	if (s.sock >= 0) {
		close(s.sock);
		s.sock = -1;
	}
}

static int enumif_open(void)
{
	int fd;
	struct sockaddr_nl local = {
		.nl_family = AF_NETLINK,
		.nl_groups = 0,
	};

	if (s.sock >= 0)
		return s.sock;

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (fd < 0) {
		perror("Cannot open netlink socket");
		return -1;
	}

	if (bind(fd, (struct sockaddr *)&local, sizeof(local)) < 0) {
		perror("Cannot bind netlink socket");
		return -1;
	}
	s.sock = fd;
	s.seq = 0;
	return fd;
}

static inline int enumif_send_request(int sk)
{
	struct {
		struct nlmsghdr n;
		struct rtgenmsg g;
	} req = {
		.n = {
			.nlmsg_len = sizeof(req),
			.nlmsg_type = RTM_GETLINK,
			.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT | NLM_F_MATCH,
			.nlmsg_pid = 0,
			.nlmsg_seq = ++s.seq,
		},
		.g.rtgen_family = AF_PACKET,
	};
	return send(sk, &req, sizeof(req), 0);
}

static void parse_rtattr(struct rtattr **tb, int max,
		struct rtattr *rta, int len)
{
	memset(tb, 0, sizeof(*tb) * max);
	while (RTA_OK(rta, len)) {
		if (rta->rta_type <= max)
			tb[rta->rta_type] = rta;
		rta = RTA_NEXT(rta, len);
	}
}

struct enumif *enumif(int if_type, int if_flags, int if_state)
{
	int ret, n_if = 0, curif_state;
	size_t msglen;
	struct nlmsghdr *h;
	struct enumif *table;
	int tablesize;
	struct sockaddr_nl peer;
	char cbuf[128];
	char buf[1024 * 16];
	struct iovec iov = {
		.iov_base = buf,
		.iov_len = sizeof(buf),
	};
	struct msghdr msg = {
		.msg_name = (void *)&peer,
		.msg_namelen = sizeof(peer),
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = &cbuf,
		.msg_controllen = sizeof(cbuf),
		.msg_flags = 0,
	};
	struct ifinfomsg *ifi;
	struct rtattr *tb[IFLA_MAX + 1];

	if (enumif_open() < 0)
		return NULL;

	if (enumif_send_request(s.sock) < 0) {
		perror("Cannot send dump request");
		return NULL;
	}

	table = NULL;
	tablesize = 0;

	while (1) {
		iov.iov_len = sizeof(buf);

		ret = recvmsg(s.sock, &msg, 0);
		if (ret < 0) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			perror("netlink receive error");
			goto failed;
		}
		if (ret == 0) {
			errno = 0;
			perror("netlink EOF?");
			goto failed;
		}
		msglen = ret;
		for (h = (void *)buf; NLMSG_OK(h, msglen);
				h = NLMSG_NEXT(h, msglen)) {
			if (h->nlmsg_type == NLMSG_DONE)
				goto done;
			if (h->nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(h);
				if (h->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
					fprintf(stderr,
						"ERROR truncated\n");
				} else {
					errno = -err->error;
					perror("RTNETLINK answers");
				}
				goto failed;
			}
			/* fetch data */
			ifi = NLMSG_DATA(h);
			parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), IFLA_PAYLOAD(h));
			curif_state = *(unsigned char *)RTA_DATA(tb[IFLA_OPERSTATE]);

			/* test matching criteria */
			if (if_type && (ifi->ifi_type != if_type))
				continue;
			if (if_flags && (~ifi->ifi_flags & if_flags))
				continue;
			if (if_state && (curif_state != if_state))
				continue;

			/* store */
			if (n_if >= tablesize) {
				struct enumif *newtable;

				tablesize += 16;
				newtable = realloc(table, sizeof(*table)*tablesize);
				if (!newtable)
					goto failed;
				table = newtable;
			}
			strcpy(table[n_if].if_name, RTA_DATA(tb[IFLA_IFNAME]));
			table[n_if].if_index = ifi->ifi_index;
			table[n_if].if_type = ifi->ifi_type;
			table[n_if].if_flags = ifi->ifi_flags;
			table[n_if].if_state = curif_state;
			++n_if;
		}
	}
done:
	/* add 'null' terminator */
	if (n_if >= tablesize) {
		struct enumif *newtable;

		tablesize += 1;
		newtable = realloc(table, sizeof(*table)*tablesize);
		if (!newtable)
			goto failed;
		table = newtable;
		memset(table+n_if, 0, sizeof(*table));
	}
	return table;
failed:
	if (table)
		free(table);
	return NULL;
}
