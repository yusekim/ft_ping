#include "ft_ping.h"

int main(int argc, char **argv)
{
	if (argc < 2)
		return (1 + printf("ping: missing host operand\nTry 'ping -?' for more information.\n"));
	else
		parseargs(argc, ++argv);
}
