#include "ft_ping.h"
#include "exec.h"
#include "ping_signal.h"

int exec_ping(t_options *options)
{
	srand48(time(NULL));
	char ipstr[INET_ADDRSTRLEN];
	t_ping_info *info;
	struct timespec now, ps_timeout, w_timeout, w_now;
	fd_set readfds;

	signal(SIGINT, sig_handler);
	ps_timeout.tv_nsec = 100000000L;
	ps_timeout.tv_sec = 0;
	if (options->flags & W_FLAG)
		clock_gettime(CLOCK_MONOTONIC_COARSE, &w_timeout);
	for (int i = 0; i < options->hosts_num; i++)
	{
		info = build_info(options, i);
		if (info == NULL)
		{
			split_free(options->hosts);
			close(options->sockfd);
			return (1);
		}
		uint16_t seqnum = 0;
		t_stat stat;
		now.tv_sec = 0;
		struct sockaddr_in *addr = (struct sockaddr_in *)info->dest_info->ai_addr;
		inet_ntop(info->dest_info->ai_family, &(addr->sin_addr), ipstr, sizeof(ipstr));
		dprintf(STDOUT_FILENO, "FT_PING %s (%s): %ld data bytes", options->hosts[i], ipstr, PACKET_SIZE - sizeof(struct icmphdr));
		if (options->flags & V_FLAG)
			dprintf(STDOUT_FILENO, ", id 0x%x = %d", options->id, options->id);
		dprintf(STDOUT_FILENO, "\n");
		set_stat(&stat);
		while(1)
		{
			if (sendpacket(options, info, &now, &seqnum, &stat) != 0)
				break;
			clock_gettime(CLOCK_MONOTONIC_COARSE, &w_now);
			if (check_ping_expired(options, info, &w_now))
				break;
			if (options->flags & W_FLAG)
			{
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
				char recvbuf[1024] = {0};
				ssize_t recvlen = recvfrom(options->sockfd, recvbuf, 1023, 0, NULL, NULL);
				struct iphdr *ip = (struct iphdr *)recvbuf;
				struct icmphdr *icmprecv = (struct icmphdr *)(recvbuf + (ip->ihl * 4));
				uint16_t recved_seqnum;
				if (calculate_cksum((void *)icmprecv, recvlen - IPHDR_SIZE) != 0)
					continue;
				if (icmprecv->type == ICMP_ECHOREPLY)
				{
					struct timespec recvnow;
					clock_gettime(CLOCK_MONOTONIC, &recvnow);
					t_list *node = list_search(info->packets, ntohs(icmprecv->un.echo.sequence));
					if (node == NULL)
						continue;
					t_packet_data *data = (t_packet_data *)(node->data);
					data->time_taken_ms = timespec_diff(data->senttime, recvnow);
					stat.recved++;
					recved_seqnum = ntohs(icmprecv->un.echo.sequence);
					dprintf(STDOUT_FILENO, "%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms",
							recvlen - IPHDR_SIZE,
							ipstr,
							recved_seqnum,
							ip->ttl,
							data->time_taken_ms);

					if (data->is_received)
					{
						dprintf(STDOUT_FILENO, " (DUP!)");
						stat.recved--;
						stat.dup_count++;
					}
					stat.sum += data->time_taken_ms;
					dprintf(STDOUT_FILENO, "\n");
					if (stat.max < data->time_taken_ms)
						stat.max = data->time_taken_ms;
					if (stat.min > data->time_taken_ms)
						stat.min = data->time_taken_ms;
					data->is_received = 1;
				}
				else if (icmprecv->type == ICMP_ECHO)
					continue;
				else
				{
					if (icmprecv->type == ICMP_TIMXCEED)
						dprintf(STDOUT_FILENO, "%ld bytes from %s: Time to live exceeded\n", recvlen - IPHDR_SIZE, ipstr);
					else if (icmprecv->type == ICMP_UNREACH && icmprecv->code == ICMP_UNREACH_HOST)
						dprintf(STDOUT_FILENO, "%ld bytes from %s: Destination Host Unreachable\n", recvlen - IPHDR_SIZE, ipstr);
					struct iphdr *itn_hdr = (struct iphdr *)((char *)icmprecv + sizeof(struct icmphdr));
					struct icmphdr *sent_icmp = (struct icmphdr *)((char *)itn_hdr + (itn_hdr->ihl * 4));
					t_list *node = list_search(info->packets, ntohs(sent_icmp->un.echo.sequence));
					if (node == NULL)
						continue;
					t_packet_data *data = (t_packet_data *)(node->data);
					data->is_received = 1;
					if (options->flags & V_FLAG)
						print_verbose(itn_hdr, sent_icmp);
				}
			}
		}
		if (options->flags & INVALID_F)
			break;
		dprintf(STDOUT_FILENO, "--- %s ft_ping: statistics ---\n", options->hosts[i]);
		print_stats(info->packets, &stat);
		info_free(info, 0);
		info = NULL;
	}
	close(options->sockfd);
	return(ping_exit(options, info, 0));
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

int sendpacket(t_options *options, t_ping_info *info, struct timespec *prev, uint16_t *seqnum, t_stat *stat)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	if (prev->tv_sec != 0 && timespec_diff(*prev, now) < 1000.0)
		return 0;
	if (options->flags & C_FLAG && info->count == 0)
		return 0;
	t_list *newnode = list_push_back(&info->packets, *seqnum);
	if (newnode == NULL)
	{
		options->flags |= INVALID_F;
		return -1;
	}
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
	info->count--;
	clock_gettime(CLOCK_MONOTONIC, prev);
	if (get_signo())
	{
		prev->tv_sec = 0;
		return 1;
	}
	return 0;
}

int check_ping_expired(t_options *options, t_ping_info *info, struct timespec *now)
{
	t_list **head = &info->packets;
	t_list *node = *head;
	int timeout = options->linger, waiting = 0;
	while (node)
	{
		t_list *next = node->next;
		t_packet_data *data = (t_packet_data *)node->data;
		if (!data->is_received)
		{
			if ((int)timespec_diff(data->senttime, *now) / 1000 > timeout)
				list_delete(head, data->val);
			else
				waiting++;
		}
		node = next;
	}
	if (options->flags & C_FLAG && info->count == 0 && waiting == 0)
		return 1;
	return 0;
}

uint16_t calculate_cksum(const void *data, size_t len)
{
	const uint16_t *buf = data;
	uint32_t sum = 0;

	while (len > 1)
	{
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += *(const uint8_t *)buf;
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t)(~sum);
}

void print_stats(t_list *head, t_stat *stat)
{
	double trans_avg = ((double)(stat->sent - (stat->recved)) / (double)stat->sent) * 100;
	dprintf(STDOUT_FILENO, "%d packets transmitted, %d packets received, ",
			stat->sent,
			stat->recved - stat->dup_count);

	if (stat->dup_count > 0)
		dprintf(STDOUT_FILENO, "+%d duplicates, ", stat->dup_count);
	dprintf(STDOUT_FILENO, "%d%% packet loss\n", (int)trans_avg);
	if (stat->recved - stat->dup_count <= 0)
		return ;
	double rt_avg, rt_stddev = 0;
	rt_avg = stat->sum / stat->recved;
	t_list *node = head;
	for (int i = 0; i < stat->recved - stat->dup_count; i++)
	{
		double val = 0;
		t_packet_data *data = (t_packet_data *)(node->data);
		if (data->time_taken_ms)
			val = data->time_taken_ms - rt_avg;
		rt_stddev += val * val;
		node = node->next;
	}
	rt_stddev = sqrt(rt_stddev / stat->recved);
	dprintf(STDOUT_FILENO, ROUND_TRIP_MSG, stat->min, rt_avg, stat->max, rt_stddev);
}
