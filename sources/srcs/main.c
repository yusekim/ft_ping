#include "ft_ping.h"
#include "parse.h"
#include "strs.h"
#include "exec.h"

int main(int argc, char **argv)
{
	t_options options;
	memset(&options, 0, sizeof(t_options));
	if (argc == 1)
	{
		dprintf(STDERR_FILENO, "ping: missing host operand\n%s", INVALID_ARG_HELP_MSG);
		return (EX_USAGE);
	}
	int res = getoptions(argv, &options);
	// print_option_info(&options);
	if (res != 0)
		return (res);
	return(exec_ping(&options));
}
