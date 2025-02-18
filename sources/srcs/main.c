#include "ft_ping.h"
#include "parse.h"
#include "strs.h"

t_options options;

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		dprintf(STDERR_FILENO, "ping: missing host operand\nTry 'ping -?' for more information.\n");
		return (EX_USAGE);
	}
	t_ping_info *ping_info = parseargs(argc, argv);

	print_ping_info(ping_info);

	if (ping_info == NULL)
	{
		split_free(options.hosts);
		return (options.flags & Q_FLAG ? 0 : 1);
	}
	srand48(time(NULL));
	// socket build... do ping...
}
