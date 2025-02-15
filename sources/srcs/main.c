#include "ft_ping.h"

t_options options;

int main(int argc, char **argv)
{
	srand48(time(NULL));
	options = parseargs(argc, ++argv);
	if (options.invalid_option)
		return (EX_USAGE);
}
