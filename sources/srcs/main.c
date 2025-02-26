#include "ft_ping.h"
#include "parse.h"
#include "strs.h"

int main(int argc, char **argv)
{
	t_options options;
	memset(&options, 0, sizeof(t_options));
	if (argc == 1)
	{
		dprintf(STDERR_FILENO, "ping: missing host operand\n%s", INVALID_ARG_HELP_MSG);
		return (EX_USAGE);
	}
	t_ping_info *ping_info = parseargs(argc, argv, &options);
	print_ping_info(ping_info, &options);
	if (ping_info == NULL)
	{
		split_free(options.hosts);
		if (options.flags & Q_FLAG && options.flags & INVALID_F)
			return (EX_USAGE);
		return (options.flags & Q_FLAG ? 0 : 1);
	}
	srand48(time(NULL));
}
