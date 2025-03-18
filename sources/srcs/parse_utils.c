#include "parse.h"
#include "strs.h"
#include <unistd.h>

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

void print_option_info(t_options *options)
{
	char optstr[] = "v?clnwt";
	for (int i = 0; i < 7; i++)
	{
		if (!(options->flags & (1 << i)))
			optstr[i] = '.';
	}
	printf("\n===========ft_ping==============\n");
	printf("options: [%s]\n", optstr);
	if (optstr[2] != '.')
		printf("\tpacket count: %d\n", options->packets_count);
	if (optstr[3] != '.')
		printf("\tlinger: %d\n", options->linger);
	if (optstr[5] != '.')
		printf("\ttimeout: %d\n", options->timeout);
	if (optstr[6] != '.')
		printf("\tttl value: %d\n", options->ttl_val);
	int len = split_len(options->hosts);
	if (len)
		printf("\narguments:\n");
	for (int i = 0; i < len; i++)
		printf("\thostname[%d]: %s\n", i, options->hosts[i]);
	printf("================================\n");
}
