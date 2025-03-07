#include "ft_ping.h"
#include "exec.h"

void *info_free(t_ping_info *info, int is_perror)
{
	if (is_perror)
		perror("ft_ping");
	slist_free(info->packets);
	if (info->pre_packets)
		free(info->pre_packets);
	if (info->pre_packets_time)
		free(info->pre_packets_time);
	if (info->dest_info)
		freeaddrinfo(info->dest_info);
	free(info);
	return NULL;
}

char *build_preload(int num, uint16_t id)
{
	char *packets = calloc(num, PACKET_SIZE * sizeof(char));
	if (packets == NULL)
		return NULL;
	for (int i = 0; i < num; i++)
	{
		struct icmphdr *icmp = (struct icmphdr *)(packets + i * PACKET_SIZE);
		icmp->type = ICMP_ECHO;
		icmp->code = 0;
		icmp->un.echo.id = id;
		icmp->un.echo.sequence = i;
		icmp->checksum = calculate_cksum((void *)icmp, PACKET_SIZE);
	}
	return packets;
}

struct timespec *build_preload_time(int num)
{
	struct timespec *timeinfo = calloc(num, sizeof(struct timespec));
	if (timeinfo == NULL)
		return NULL;
	return timeinfo;
}

struct addrinfo *getdestinfo(char *hostname)
{
	struct addrinfo hints, *res;
	int status = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	status = getaddrinfo(hostname, NULL, &hints, &res);
	if (status != 0)
	{
		dprintf(STDERR_FILENO, "ft_ping: unknown host\n");
		return NULL;
	}
	return res;
}
