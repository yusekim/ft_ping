#ifndef PING_SIGNAL_H
# define PING_SIGNAL_H
# include <signal.h>

void	sig_handler(int signo);
void	signal_ign(void);
void	signal_dfl(void);
void	set_signo(void);
int		get_signo(void);
#endif
