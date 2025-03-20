#include "ft_ping.h"
#include "parse.h"
#include "strs.h"

int getoptions(char **argv, t_options *options)
{
	char *arg;

	while (!(options->flags & Q_FLAG || options->flags & INVALID_F) && *(++argv))
	{
		arg = *argv;
		if (*arg == '-')
			handle_options(options, arg, &argv);
		else
			options->hosts = add_str(options->hosts, strdup(arg));
	}
	if (options->flags & Q_FLAG && options->flags & INVALID_F)
		return (EX_USAGE);
	else if (options->flags & INVALID_F)
		return 1;
	else if (options->flags & Q_FLAG)
		return 0;
	options->hosts_num = split_len(options->hosts);
	if (options->hosts_num == 0)
	{
		dprintf(STDERR_FILENO, "ft_ping: missing host operand\n%s\n", INVALID_ARG_HELP_MSG);
		return (EX_USAGE);
	}
	options->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (options->sockfd < 0)
	{
		perror("ft_ping");
		return 1;
	}
	int optval = 1;
	if (setsockopt(options->sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < 0) {
		perror("ft_ping");
		return 1;
	}
	if (options->flags & TTL_FLAG)
	{
		if (setsockopt(options->sockfd, IPPROTO_IP, IP_TTL, &options->ttl_val, sizeof(uint8_t)) < 0)
		{
			perror("ft_ping");
			return 1;
		}
	}
	options->id = getpid();
	if (!(options->flags & CW_FLAG))
		options->linger = MAX_WAIT;
	return 0;
}

void handle_options(t_options *options, char *arg, char ***argv)
{
	int len = strlen(arg);
	for (int i = 1; i < len; i++)
	{
		switch (arg[i])
		{
		case '?':
			options->flags |= Q_FLAG;
			dprintf(STDOUT_FILENO, "%s", PING_USAGE);
			return;
		case 'v':
			options->flags |= V_FLAG;
			break;
		case 'c':
			options->flags |= C_FLAG;
			if (get_opt_val(options, arg[i], argv))
				return;
			break;
		case 'n':
			options->flags |= N_FLAG;
			break;
		case 'W':
			options->flags |= CW_FLAG;
			if (get_opt_val(options, arg[i], argv))
				return;
			break;
		case 'w':
			options->flags |= W_FLAG;
			if (get_opt_val(options, arg[i], argv))
				return;
			break;
		case '-':
			options->flags |= TTL_FLAG;
			get_ttl_val(options, &(arg[i]));
			return;
		default:
			options->flags |= INVALID_F | Q_FLAG;
			dprintf(STDERR_FILENO, "%s option -- '%c'\n%s", INVALID_MSG, arg[i], INVALID_ARG_HELP_MSG);
			return;
		}
	}
}

int get_opt_val(t_options *options, char flag, char ***argv)
{
	++(*argv);
	if (!*argv || !**argv)
	{
		options->flags |= Q_FLAG | INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: option requires an argument -- '%c'\n%s\n", flag, INVALID_ARG_HELP_MSG);
		return 1;
	}
	char *check = is_ascii_number(**argv);
	if (check)
	{
		options->flags |= INVALID_F;
		if (flag == 'l')
			dprintf(STDERR_FILENO, "%s preload value (%s)\n", INVALID_MSG, **argv);
		else
			dprintf(STDERR_FILENO, "%s value (`%s' near `%s')\n", INVALID_MSG, **argv, check);
		return 1;
	}
	long value = atol(**argv);
	if (value < 0 || value > INT32_MAX)
	{
		if (flag == 'c')
		{
			options->flags ^= C_FLAG;
			return 0;
		}
		options->flags |= INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: option value too big: %s\n", **argv);
		return 1;
	}
	else if (value == 0)
	{
		options->flags |= INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: option value too small: %s\n", **argv);
		return 1;
	}

	switch (flag)
	{
	case 'W':
		options->linger = (int)value;
		break;
	case 'w':
		options->timeout = (int)value;
		break;
	case 'c':
		options->packets_count = (uint16_t)value;
	}
	return 0;
}

void get_ttl_val(t_options *options, char *flag)
{
	if (strncmp("-ttl=", flag, 5))
	{
		options->flags |= Q_FLAG | INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: unrecognized option '-%s'\n%s\n", flag, INVALID_ARG_HELP_MSG);
		return;
	}
	flag += 5;
	char *check = is_ascii_number(flag);
	if (check)
	{
		options->flags |= INVALID_F;
		dprintf(STDERR_FILENO, "%s value (`%s' near `%s')\n", INVALID_MSG, flag, check);
		return;
	}
	int value = atoi(flag);
	if (value < 0 || value > UINT8_MAX)
	{
		options->flags |= INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: option value too big: %s\n", flag);
	}
	else if (value == 0)
	{
		options->flags |= INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: option value too small: %s\n", flag);
	}
	else
		options->ttl_val = (uint8_t)value;
	return;
}

