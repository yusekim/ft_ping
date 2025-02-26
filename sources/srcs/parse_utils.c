#include "parse.h"
#include "strs.h"

/*
yusekim@debian:~$ ping -c 1 --ttl=1 google.com
PING google.com (172.217.25.174): 56 data bytes
36 bytes from 192.168.64.1: Time to live exceeded
--- google.com ping statistics ---
1 packets transmitted, 0 packets received, 100% packet loss

yusekim@debian:~$ ping -vc 1 --ttl=1 google.com
PING google.com (172.217.25.174): 56 data bytes, id 0x07ae = 1966
36 bytes from 192.168.64.1: Time to live exceeded
IP Hdr Dump:
 4500 0054 c9a4 0000 0101 28d2 c0a8 4003 acd9 19ae
Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
 4  5  00 0054 c9a4   0 0000  01  01 28d2 192.168.64.3  172.217.25.174
ICMP: type 8, code 0, size 64, id 0x07ae, seq 0x0000
--- google.com ping statistics ---
1 packets transmitted, 0 packets received, 100% packet loss

yusekim@debian:~$ ping -vc 1 --ttl=100 google.com
PING google.com (172.217.25.174): 56 data bytes, id 0x07af = 1967
64 bytes from 172.217.25.174: icmp_seq=0 ttl=116 time=120.373 ms
--- google.com ping statistics ---
1 packets transmitted, 1 packets received, 0% packet loss
round-trip min/avg/max/stddev = 120.373/120.373/120.373/0.000 ms

yusekim@debian:~$ ping -vc 1 google.com
PING google.com (142.250.76.142): 56 data bytes, id 0x07b2 = 1970
64 bytes from 142.250.76.142: icmp_seq=0 ttl=115 time=50.754 ms
--- google.com ping statistics ---
1 packets transmitted, 1 packets received, 0% packet loss
round-trip min/avg/max/stddev = 50.754/50.754/50.754/0.000 ms
*/

#include <unistd.h>

void *info_free(t_ping_info *info, int is_perror)
{
	t_ping_info *temp;
	if (is_perror)
		perror("ft_ping");
	while (info)
	{
		temp = info->next;
		// slist_free(temp->packets); // TODO
		if (info->pre_packets)
			free(info->pre_packets);
		free(info);
		write(1, "HERE6\n", 6);
		info = temp;
	}
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

char *is_ascii_number(char *str)
{
	size_t len = strlen(str);
	if (*str == '-')
	{
		if (len == 1)
			return str;
		str++;
		len--;
	}
	for (size_t i = 0; i < len; i++)
	{
		if (str[i] < '0' || str[i] > '9')
			return (str + i);
	}
	return NULL;
}

void print_ping_info(t_ping_info *info, t_options *options)
{
	if (info == NULL)
		return ;
	char optstr[] = "v?flnwW";
	for (int i = 0; i < strlen(optstr); i++)
	{
		if (!(options->flags & (1 << i)))
			optstr[i] = '.';
	}
	printf("\n===========ft_ping==============\n");
	printf("options: [%s]\n", optstr);
	if (optstr[3] != '.')
		printf("\tpreload: %d\n", options->preload_num);
	if (optstr[5] != '.')
		printf("\ttimeout: %d\n", options->timeout);
	if (optstr[6] != '.')
		printf("\tttl value: %d\n", options->ttl_val);

	int len = split_len(options->hosts);
	if (len)
		printf("\narguments:\n");
	for (int i = 0; i < len; i++)
		printf("\t%s\n", options->hosts[i]);
	printf("================================\n");

}
