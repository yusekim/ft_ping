#include "ft_ping.h"
#include "parse.h"
#include "strs.h"

// ping -> stderr
// ping -Z -> stderr
// ping -?Z google.com -> stdout
// ping -Z? google.com -> stderr
// ping -QZ google.com -> stderr
// ping -cl 2 3 google.com -> 문제없어보이면 stdout
// ping -w 0 8.8.8.8 -> stderr: "ping: option value too small: 0"

t_ping_info *parseargs(int argc, char **argv, t_options *options)
{
	t_ping_info *cur;
	char *arg;
	int len;

	while (!(options->flags & Q_FLAG || options->flags & INVALID_F) && *(++argv))
	{
		arg = *argv;
		if (*arg == '-')
			handle_options(options, arg, &argv);
		else
			options->hosts = add_str(options->hosts, strdup(arg));
	}
	if (options->flags & Q_FLAG || options->flags & INVALID_F)
		return NULL;
	len = split_len(options->hosts);
	if (len == 0)
	{
		options->flags |= Q_FLAG | INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: missing host operand\n%s\n", INVALID_ARG_HELP_MSG);
		return NULL;
	}
	return (build_info(options, len));
}

int getoptions(int argc, char **argv, t_options *options)
{
	char *arg;
	int len;

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
	len = split_len(options->hosts);
	if (len == 0)
	{
		dprintf(STDERR_FILENO, "ft_ping: missing host operand\n%s\n", INVALID_ARG_HELP_MSG);
		return (EX_USAGE);
	}
	return 0;
}


t_ping_info *build_info(t_options *options, int len) // TODO build t_ping_info linked-list
{
	t_ping_info *head = calloc(1, sizeof(t_ping_info));
	if (head == NULL)
		return info_free(head, 1);
	t_ping_info *temp = head;
	uint16_t id = getpid();
	int idx = 0;
	while (temp)
	{
		if (len - idx > 1)
		{
			temp->next = calloc(1, sizeof(t_ping_info));
			if (temp->next == NULL)
				return info_free(head, 1);
		}
		temp->dest_info = getdestinfo(options->hosts[idx]);
		if (temp->dest_info == NULL)
			return (info_free(head, 0));
		temp->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (temp->sockfd < 0)
			return info_free(head, 1);
		if (options->preload_num)
		{
			temp->pre_packets = build_preload(options->preload_num, id);
			if (temp->pre_packets == NULL)
				return info_free(head, 1);
		}
		temp = temp->next;
		idx++;
	}
	return head;
}

void handle_options(t_options *options, char *arg, char ***argv)
{
	size_t len = strlen(arg);
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
		case 'l':
			options->flags |= L_FLAG;
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
			if (get_ttl_val(options, &(arg[i]), &i)) // TODO
				return;
			break;
		default:
			options->flags |= INVALID_F;
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
		options->flags |= INVALID_F;
		if (flag == 'l')
			dprintf(STDERR_FILENO, "%s preload value (%s)\n", INVALID_MSG, **argv);
		else
			dprintf(STDERR_FILENO, "ft_ping: option value too big: %s\n", **argv);
		return 1;
	}
	else if (value == 0 && !(flag == 'l'))
	{
		options->flags |= INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: option value too small: %s\n", **argv);
		return 1;
	}

	switch (flag)
	{
	case 'l':
		options->preload_num = (int)value;
		break;
	case 'w':
		options->timeout = (int)value;
		break;
	}
	return 0;
}

int get_ttl_val(t_options *options, char *flag, int *idx)
{
	if (strncmp("-ttl=", flag, 5))
	{
		options->flags |= Q_FLAG | INVALID_F;
		dprintf(STDERR_FILENO, "ping: ping: unrecognized option '-%s'\n%s\n", flag, INVALID_ARG_HELP_MSG);
		return 1;
	}
	flag += 5;
	char *check = is_ascii_number(flag);
	if (check)
	{
		options->flags |= INVALID_F;
		dprintf(STDERR_FILENO, "%s value (`%s' near `%s')\n", INVALID_MSG, flag, check);
		return 1;
	}
	int value = atoi(flag);
	if (value < 0 || value > UINT8_MAX)
	{
		options->flags |= INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: option value too big: %s\n", flag);
		return 1;
	}
	else if (value == 0)
	{
		options->flags |= INVALID_F;
		dprintf(STDERR_FILENO, "ft_ping: option value too small: %s\n", flag);
		return 1;
	}
	options->ttl_val = (uint8_t)value;
}
