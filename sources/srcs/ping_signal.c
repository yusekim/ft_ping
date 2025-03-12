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

void on_signo(void)
{
	g_status = 1;
}

void off_signo(void)
{
	g_status = 0;
}

int get_signo(void)
{
	return g_status;
}

