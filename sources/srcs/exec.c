#include "ft_ping.h"
#include "exec.h"
#include "ping_signal.h"

int exec_ping(t_options *options)
{
	t_ping_info *info;
	int len = split_len(options->hosts);
	srand48(time(NULL));
	for (int i = 0; i < len; i++)
	{
		info = build_info(options, i);
		if (!info)
		{
			split_free(options->hosts);
			return 1;
		}
		send_preloads(info); // TODO





		//
		// int ttl = 5;	// to raise ttl exceed
		// setsockopt(options->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

		char packet[64] = {0};
		struct icmphdr *icmp = (struct icmphdr *)(packet);
		icmp->type = ICMP_ECHO;
		icmp->code = 0;
		icmp->un.echo.id = htons(getpid());
		icmp->un.echo.sequence = htons(31);
		icmp->checksum = calculate_cksum((void *)icmp, PACKET_SIZE);
		sendto(options->sockfd, packet, 64, 0, info->dest_info->ai_addr, info->dest_info->ai_addrlen);


		char buffer[1024];
		ssize_t recvlen = recvfrom(options->sockfd, buffer, 1023, 0, NULL, NULL);
		struct iphdr *ip = (struct iphdr *)buffer;
		struct icmphdr *icmp2 = (struct icmphdr *)(buffer + (ip->ihl * 4));


		printf("icmp2 checksume: %d\n", calculate_cksum((void *)icmp2, PACKET_SIZE));
		printf("icmp2->type: %d\n", icmp2->type);
		printf("icmp2->code: %d\n", icmp2->code);

		exit(recvlen);
		//
	}
	return 0;
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
