#include "ft_ping.h"
#include "exec.h"
#include "ping_signal.h"

int exec_ping(t_options *options)
{
	srand48(time(NULL));
	int len = split_len(options->hosts);
	t_ping_info *info;

	struct timespec timeout;
	for (int i = 0; i < len; i++)
	{
		info = build_info(options, i);
		if (!info)
		{
			split_free(options->hosts);
			return 1;
		}
		// send_preloads(info); // TODO
		signal(SIGINT, sig_handler);
		struct timespec now, timeout;
		timeout.tv_sec = 1;
		timeout.tv_nsec = 1000000L;
		fd_set readfds;
		uint16_t seqnum = 0;
		clock_gettime(CLOCK_MONOTONIC, &now);
		while(1)
		{
			if (sendpacket(options, info, &now, &seqnum) < 0)
				return ping_exit(options, info, 1);
			FD_ZERO(&readfds);
			FD_SET(options->sockfd, &readfds);
			int ret = pselect(options->sockfd + 1, &readfds, NULL, NULL, &timeout, NULL);
			if (ret < 0)
				return ping_exit(options, info, 1);
			else if (ret > 0)
			{
				char recvbuf[1024];
				ssize_t recvlen = recvfrom(options->sockfd, recvbuf, 1023, 0, NULL, NULL);
				struct iphdr *ip = (struct iphdr *)recvbuf;
				struct icmphdr *icmprecv = (struct icmphdr *)(recvbuf + (ip->ihl * 4));
				if (calculate_cksum((void *)icmprecv, PACKET_SIZE) == 0)
				{
					t_slist *node = slist_search(info->packets, icmprecv->un.echo.sequence);
					dprintf(STDOUT_FILENO, "SEQ: %d\n", icmprecv->un.echo.sequence);
					struct timespec recvnow;
					clock_gettime(CLOCK_MONOTONIC, &recvnow);
					node->time_taken_ms = timespec_diff(node->senttime, recvnow);
					dprintf(STDOUT_FILENO, "ping recv: %f\n", node->time_taken_ms);
				}
			}
		}
	}
	close(options->sockfd);
	return 0;
}

int sendpacket(t_options *options, t_ping_info *info, struct timespec *prev, uint16_t *seqnum)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	if (timespec_diff(*prev, now) < 1000.0)
		return (0);
	t_slist *newnode = slist_push_back(&info->packets, *seqnum);
	char packet[64] = {0};
	struct icmphdr *icmp = (struct icmphdr *)(packet);
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = htons(options->id);
	icmp->un.echo.sequence = *seqnum;
	icmp->checksum = calculate_cksum((void *)icmp, PACKET_SIZE);
	sendto(options->sockfd, packet, 64, 0, info->dest_info->ai_addr, info->dest_info->ai_addrlen);
	clock_gettime(CLOCK_MONOTONIC, prev);
	(*seqnum)++;
	if (get_signo())
	{
		dprintf(STDOUT_FILENO, "ft_ping: statsprint\n");
		exit(42);
	}
	return (1);
}


t_ping_info *build_info(t_options *options, int idx)
{
	t_ping_info *info = calloc(1, sizeof(t_ping_info));
	if (info == NULL)
		return info_free(info, 1);
	uint16_t id = options->id;
	info->dest_info = getdestinfo(options->hosts[idx]);
	if (info->dest_info == NULL)
		return (info_free(info, 0));
	if (options->flags & L_FLAG)
	{
		info->pre_packets = build_preload(options->preload_num, id);
		info->pre_packets_time = build_preload_time(options->preload_num);
		if (info->pre_packets_time == NULL || info->pre_packets == NULL)
			return (info_free(info, 1));
	}
	return info;
}
