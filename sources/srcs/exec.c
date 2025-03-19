#include "ft_ping.h"
#include "exec.h"
#include "ping_signal.h"

int exec_ping(t_options *options)
{
	srand48(time(NULL));
	int len = split_len(options->hosts);
	char ipstr[INET_ADDRSTRLEN];
	t_ping_info *info;
	struct timespec now, ps_timeout, w_timeout, w_now;
	fd_set readfds;

	signal(SIGINT, sig_handler);
	ps_timeout.tv_nsec = 50000000L;
	ps_timeout.tv_sec = 0;
	for (int i = 0; i < len; i++)
	{
		info = build_info(options, i);
		if (!info)
			ping_exit(options, info, 1);
		uint16_t seqnum = 0;
		t_stat stat;
		now.tv_sec = 0;
		if (options->flags & W_FLAG)
			clock_gettime(CLOCK_MONOTONIC_COARSE, &w_timeout);
		struct sockaddr_in *addr = (struct sockaddr_in *)info->dest_info->ai_addr;
		inet_ntop(info->dest_info->ai_family, &(addr->sin_addr), ipstr, sizeof(ipstr));
		dprintf(STDOUT_FILENO, "FT_PING %s (%s): %d data bytes", options->hosts[i], ipstr, PACKET_SIZE - sizeof(struct icmphdr));
		if (options->flags & V_FLAG)
			dprintf(STDOUT_FILENO, ", id 0x%x = %d", options->id, options->id);
		dprintf(STDOUT_FILENO, "\n");
		set_stat(&stat);
		while(1)
		{
			if (sendpacket(i, options, info, &now, &seqnum, &stat) < 0)
				break;
			if (options->flags & W_FLAG)
			{
				clock_gettime(CLOCK_MONOTONIC_COARSE, &w_now);
				if (timespec_diff(w_timeout, w_now) / 1000 > options->timeout)
					break;
			}
			FD_ZERO(&readfds);
			FD_SET(options->sockfd, &readfds);
			int ret = pselect(options->sockfd + 1, &readfds, NULL, NULL, &ps_timeout, NULL);
			if (ret < 0)
			{
				if (errno == EINTR)
					break;
				return ping_exit(options, info, 1);
			}
			else if (ret > 0)
			{
				char recvbuf[1024];
				ssize_t recvlen = recvfrom(options->sockfd, recvbuf, 1023, 0, NULL, NULL);
				struct iphdr *ip = (struct iphdr *)recvbuf;
				struct icmphdr *icmprecv = (struct icmphdr *)(recvbuf + (ip->ihl * 4));
				uint16_t recved_seqnum;
				if (icmprecv->type == ICMP_TIMXCEED)
				{
					dprintf(STDOUT_FILENO, "%d bytes from %s: Time to live exceeded\n", recvlen - IPHDR_SIZE, ipstr);
					struct icmphdr *sent_packet = (struct icmphdr *)((char *)icmprecv + 8);
					recved_seqnum = ntohs(sent_packet->un.echo.sequence);
					if (options->flags & V_FLAG)
						print_verbose((char *)icmprecv);
				}
				else if (icmprecv->type == ICMP_ECHOREPLY && calculate_cksum((void *)icmprecv, recvlen - IPHDR_SIZE) == 0)
				{
					struct timespec recvnow;
					clock_gettime(CLOCK_MONOTONIC, &recvnow);
					t_slist *node = slist_search(info->packets, ntohs(icmprecv->un.echo.sequence));
					node->time_taken_ms = timespec_diff(node->senttime, recvnow);
					if ((uint16_t)(node->time_taken_ms) / 1000 > options->linger)
						continue ;
					stat.recved++;
					if (node->time_taken_ms > options->linger)
					recved_seqnum = ntohs(icmprecv->un.echo.sequence);
					dprintf(STDOUT_FILENO, "%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms",
							recvlen - IPHDR_SIZE,
							ipstr,
							recved_seqnum,
							ip->ttl,
							node->time_taken_ms);

					if (node->is_received)
					{
						dprintf(STDOUT_FILENO, " (DUP!)");
						stat.dup_count++;
					}
					stat.sum += node->time_taken_ms;
					dprintf(STDOUT_FILENO, "\n");
					if (stat.max < node->time_taken_ms)
						stat.max = node->time_taken_ms;
					if (stat.min > node->time_taken_ms)
						stat.min = node->time_taken_ms;
					node->is_received = 1;
					memset(recvbuf, 0, 1024);
				}
				if (options->flags & C_FLAG)
				{
					info->count--;
					if (info->count == 0)
						break ;
				}
			}
		}
		dprintf(STDOUT_FILENO, "--- %s ft_ping: statistics ---\n", options->hosts[i]);
		print_stats(info->packets->level_ptrs[0], &stat);
	}
	close(options->sockfd);
	ping_exit(options, info, 0);
}

int sendpacket(int idx, t_options *options, t_ping_info *info, struct timespec *prev, uint16_t *seqnum, t_stat *stat)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	if (prev->tv_sec != 0 && timespec_diff(*prev, now) < 1000.0)
		return 0;
	t_slist *newnode = slist_push_back(&info->packets, *seqnum);
	char packet[PACKET_SIZE] = {0};
	struct icmphdr *icmp = (struct icmphdr *)(packet);
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = htons(options->id);
	icmp->un.echo.sequence = htons(*seqnum);
	*seqnum += 1;
	icmp->checksum = calculate_cksum((void *)icmp, PACKET_SIZE);
	if (sendto(options->sockfd, packet, PACKET_SIZE, 0, info->dest_info->ai_addr, info->dest_info->ai_addrlen) < 0)
		ping_exit(options, info, 1);
	stat->sent++;
	clock_gettime(CLOCK_MONOTONIC, prev);
	if (get_signo())
	{
		prev->tv_sec = 0;
		return -1;
	}
	return 0;
}

t_ping_info *build_info(t_options *options, int idx)
{
	t_ping_info *info = calloc(1, sizeof(t_ping_info));
	if (info == NULL)
		return info_free(info, 1);
	info->dest_info = getdestinfo(options->hosts[idx]);
	if (info->dest_info == NULL)
		return (info_free(info, 0));
	if (options->flags & C_FLAG)
		info->count = options->packets_count;
	return info;
}

void print_stats(t_slist *head, t_stat *stat)
{
	double trans_avg = ((double)(stat->sent - (stat->recved - stat->dup_count)) / (double)stat->sent) * 100;
	dprintf(STDOUT_FILENO, "%d packets transmitted, %d packets received, ",
			stat->sent,
			stat->recved - stat->dup_count);

	if (stat->dup_count > 0)
		dprintf(STDOUT_FILENO, "+%d duplicates, ", stat->dup_count);
	dprintf(STDOUT_FILENO, "%d%% packet loss\n", (int)trans_avg);
	if (stat->recved - stat->dup_count == 0)
		return ;
	double rt_avg, rt_stddev = 0;
	rt_avg = stat->sum / stat->recved;
	for (int i = 0; i < stat->recved - stat->dup_count; i++)
	{
		double val = 0;
		if (head->time_taken_ms)
			val = head->time_taken_ms - rt_avg;
		rt_stddev += val * val;
		head = head->level_ptrs[0];
	}
	rt_stddev = sqrt(rt_stddev / stat->recved);
	dprintf(STDOUT_FILENO, ROUND_TRIP_MSG, stat->min, rt_avg, stat->max, rt_stddev);
}
