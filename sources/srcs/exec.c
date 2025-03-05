#include "ft_ping.h"
#include "exec.h"

int exec_ping(t_options *options)
{
	t_ping_info *info;
	int len = split_len(options->hosts);
	for (int i = 0; i < len; i++)
	{
		info = build_info(options, i);
		if (!info)
		{
			split_free(options->hosts);
			return 1;
		}
		// TODO: send and receive packets...
	}
	return 0;
}

t_ping_info *build_info(t_options *options, int idx)
{
	t_ping_info *info = calloc(1, sizeof(t_ping_info));
	if (info == NULL)
		return info_free(info, 1);
	uint16_t id = getpid();
	info->dest_info = getdestinfo(options->hosts[idx]);
	if (info->dest_info == NULL)
		return (info_free(info, 0));
	struct addrinfo *addr = info->dest_info;
	info->sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (info->sockfd < 0)
		return info_free(info, 1);
	if (options->preload_num)
	{
		info->pre_packets = build_preload(options->preload_num, id);
		if (info->pre_packets == NULL)
			return (info_free(info, 1));
	}
	return info;
}
