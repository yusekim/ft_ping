#include "ping_signal.h"

volatile sig_atomic_t	g_status = 0;

void sig_handler(int signo)
{
	g_status = signo;
}

void signal_ign(void)
{
	signal(SIGINT, SIG_IGN);
}

void signal_dfl(void)
{
	signal(SIGINT, SIG_DFL);
}

void set_signo(void)
{
	g_status = 1;
}

int get_signo(void)
{
	return g_status;
}
