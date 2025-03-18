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

void print_verbose(char *icmprecv)
{
	// 외부 ICMP 헤더 바로 뒤에 있는 원본 IP 헤더를 가져옴.
	struct iphdr *orig_ip = (struct iphdr *)((char *)icmprecv + sizeof(struct icmphdr));
	uint16_t *dump = (uint16_t *)orig_ip;

	// 20바이트(10워드)의 원본 IP 헤더 dump 출력
	dprintf(STDOUT_FILENO, "IP Hdr Dump:\n");
	for (int i = 0; i < 10; i++)
		dprintf(STDOUT_FILENO, " %04x", ntohs(dump[i]));
	dprintf(STDOUT_FILENO, "\n" HDR_DUMP_MSG);

	// 원본 IP 헤더 필드 파싱
	int version = orig_ip->version;
	int ihl = orig_ip->ihl;
	int tos = orig_ip->tos;
	uint16_t tot_len = ntohs(orig_ip->tot_len);
	uint16_t id = ntohs(orig_ip->id);
	uint16_t frag = ntohs(orig_ip->frag_off);
	int flags = frag >> 13;
	int frag_off = frag & 0x1fff;
	int ttl = orig_ip->ttl;
	int protocol = orig_ip->protocol;
	uint16_t checksum = ntohs(orig_ip->check);

	struct in_addr src, dst;
	char src_str[INET_ADDRSTRLEN], dst_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(orig_ip->saddr), src_str, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(orig_ip->daddr), dst_str, INET_ADDRSTRLEN);

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

	// 원본 IP 헤더 뒤에 오는 원본 ICMP 헤더를 정확하게 얻음.
	struct icmphdr *orig_icmp = (struct icmphdr *)((char *)orig_ip + (ihl * 4));
	int icmp_type = orig_icmp->type;
	int icmp_code = orig_icmp->code;
	// 원본 ICMP 패킷의 크기는 원본 전체 IP 길이에서 IP 헤더 크기를 뺀 값 (보통 56바이트 또는 64바이트)
	int icmp_size = tot_len - (ihl * 4);

	dprintf(STDOUT_FILENO,
		"ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n",
		icmp_type, icmp_code, icmp_size, ntohs(orig_icmp->un.echo.id), ntohs(orig_icmp->un.echo.sequence));
}


double timespec_diff(struct timespec start, struct timespec end)
{
	double res = (end.tv_sec - start.tv_sec) * 1000.0;
	res += (end.tv_nsec - start.tv_nsec) / 1000000.0;
	return res;
}
