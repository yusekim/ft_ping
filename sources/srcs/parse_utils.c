#include "ft_ping.h"
#include "parse.h"
#include "strs.h"

extern t_options options;

// ping -> stderr
// ping -Z -> stderr
// ping -?Z google.com -> stdout
// ping -Z? google.com -> stderr
// ping -QZ google.com -> stderr
// ping -cl 2 3 google.com -> 문제없어보이면 stdout
// ping -w 0 8.8.8.8 -> stderr: "ping: option value too small: 0"

//
void *ping_free(t_ping_info *info)
{
	free(info);
	return 0;
}
//


t_ping_info *parseargs(int argc, char **argv)
{
	t_ping_info *info = malloc(sizeof(t_ping_info));
	if (info == NULL)
		return NULL;
	char *arg;

	while (!(options.flags & Q_FLAG || options.flags & INVALID_F) && *(++argv))
	{
		arg = *argv;
		if (*arg == '-')
			handle_options(info, arg, &argv);
		else
			options.hosts = add_str(options.hosts, strdup(arg));
	}
	if (options.flags & Q_FLAG || options.flags & INVALID_F)
		return ping_free(info);
	return info;
}

void handle_options(t_ping_info *info, char *arg, char ***argv)
{
	size_t len = strlen(arg);
	for (int i = 1; i < len; i++)
	{
		switch (arg[i])
		{
		case '?':
			options.flags |= Q_FLAG;
			dprintf(STDOUT_FILENO, "%s", PING_USAGE);
			return;
		case 'v':
			options.flags |= V_FLAG;
			break;
		case 'f':
			options.flags |= F_FLAG;
			break;
		case 'n':
			options.flags |= N_FLAG;
			break;
		case 'l':
			options.flags |= L_FLAG;
			if (get_opt_val(info, arg[i], argv)) // 예외상황 발생시 INVALID_F 비트 키고 메세지 출력
				return;
			break;
		case 'w':
			options.flags |= W_FLAG;
			if (get_opt_val(info, arg[i], argv))
				return;
			break;
		case 'W':
			options.flags |= CW_FLAG;
			if (get_opt_val(info, arg[i], argv))
				return;
			break;
		default:
			options.flags |= INVALID_F;
			dprintf(STDERR_FILENO, "%s option -- '%c'\n%s", INVALID_MSG, arg[i], INVALID_ARG_HELP_MSG);
			return;
		}
	}
}

char *is_ascii_number(char *str)
{
	if (*str == '-')
		str++;
	size_t len = strlen(str);
	for (size_t i = 0; i < len; i++)
	{
		if (str[i] < '0' || str[i] > '9')
			return (&str[i]);
	}
	return NULL;
}

int get_opt_val(t_ping_info *info, char flag, char ***argv)
{
	++(*argv);
	char *check = is_ascii_number(**argv);
	if (check)
	{
		options.flags |= INVALID_F;
		if (flag == 'l')
			dprintf(STDERR_FILENO, "%s preload value (%s)\n", INVALID_MSG, **argv);
		else
			dprintf(STDERR_FILENO, "%s value (`%s' near `%s')\n", INVALID_MSG, **argv, check);
		return 1;
	}
	long value = atol(**argv);
	if (value < 0 || value > INT32_MAX)
	{
		options.flags |= INVALID_F;
		if (flag == 'l')
			dprintf(STDERR_FILENO, "%s preload value (%s)\n", INVALID_MSG, **argv);
		else
			dprintf(STDERR_FILENO, "ping: option value too big: %s\n", **argv);
		return 1;
	}
	else if (value == 0 && !(flag == 'l'))
	{
		options.flags |= INVALID_F;
		dprintf(STDERR_FILENO, "ping: option value too small: %s\n", **argv);
		return 1;
	}

	switch (flag)
	{
	case 'l':
		info->l_flag_val = (int)value;
		break;
	case 'w':
		info->w_flag_val = (int)value;
		break;
	case 'W':
		info->W_flag_val = (int)value;
	}
	return 0;
}

void print_ping_info(t_ping_info *info)
{
	char optstr[] = "v?flnwW";
	for (int i = 0; i < strlen(optstr); i++)
	{
		if (!(options.flags & (1 << i)))
			optstr[i] = '.';
	}
	printf("\n===========ft_ping==============\n");
	printf("options: [%s]\n", optstr);
	if (info && optstr[3] != '.')
		printf("\tpreload: %d\n", info->l_flag_val);
	if (info && optstr[5] != '.')
		printf("\ttimeout: %d\n", info->w_flag_val);
	if (info && optstr[6] != '.')
		printf("\tlinger: %d\n", info->W_flag_val);

	int len = split_len(options.hosts);
	if (len)
		printf("\narguments:\n");
	for (int i = 0; i < len; i++)
		printf("\t%s\n", options.hosts[i]);
	printf("===============================\n");
}
