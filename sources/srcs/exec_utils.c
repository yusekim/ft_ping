#include "ft_ping.h"
#include "exec.h"
#include <float.h>

void *info_free(t_ping_info *info, int is_perror)
{
	if (is_perror)
		perror("ft_ping");
	if (!info)
		return NULL;
	slist_free(info->packets);
	if (info->dest_info)
		freeaddrinfo(info->dest_info);
	free(info);
	return NULL;
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

int ping_exit(t_options *options, t_ping_info *info, int code)
{
	info_free(info, code);
	split_free(options->hosts);
	close(options->sockfd);
	return code;
}

void set_stat(t_stat *stat)
{
	memset(stat, 0, sizeof(t_stat));
	stat->min = DBL_MAX;
	stat->max = DBL_MIN;
}

void print_verbose(struct iphdr *ip_hdr, struct icmphdr *sent_icmp)
{
	uint16_t *dump = (uint16_t *)ip_hdr;

	dprintf(STDOUT_FILENO, "IP Hdr Dump:\n");
	for (int i = 0; i < 10; i++)
		dprintf(STDOUT_FILENO, " %04x", ntohs(dump[i]));
	dprintf(STDOUT_FILENO, HDR_DUMP_MSG);

	int version = ip_hdr->version;
	int ihl = ip_hdr->ihl;
	int tos = ip_hdr->tos;
	uint16_t tot_len = ntohs(ip_hdr->tot_len);
	uint16_t id = ntohs(ip_hdr->id);
	uint16_t frag = ntohs(ip_hdr->frag_off);
	int flags = frag >> 13;
	int frag_off = frag & 0x1fff;
	int ttl = ip_hdr->ttl;
	int protocol = ip_hdr->protocol;
	uint16_t checksum = ntohs(ip_hdr->check);

	char src_str[INET_ADDRSTRLEN], dst_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ip_hdr->saddr), src_str, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(ip_hdr->daddr), dst_str, INET_ADDRSTRLEN);

	dprintf(STDOUT_FILENO,
		" %d  %d  %02x %04x %04x   %d %04x  %02x  %02x %04x %s  %s\n",
		version,		 // Version (예: 4)
		ihl,			 // IHL (예: 5)
		tos,			 // TOS
		tot_len,		 // Total Length
		id,			  // Identification
		flags,		   // Flags
		frag_off,		// Fragment Offset
		ttl,			 // TTL
		protocol,		// Protocol
		checksum,		// Checksum
		src_str,  // Source IP (예: 192.168.64.3)
		dst_str); // Destination IP (예: 142.250.206.206)

	int icmp_type = sent_icmp->type;
	int icmp_code = sent_icmp->code;
	int icmp_size = tot_len - (ihl * 4);

	dprintf(STDOUT_FILENO,
		"ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n",
		icmp_type, icmp_code, icmp_size, ntohs(sent_icmp->un.echo.id), ntohs(sent_icmp->un.echo.sequence));
}


double timespec_diff(struct timespec start, struct timespec end)
{
	double res = (end.tv_sec - start.tv_sec) * 1000.0;
	res += (end.tv_nsec - start.tv_nsec) / 1000000.0;
	return res;
}
