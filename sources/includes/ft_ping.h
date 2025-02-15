#ifndef FT_PING_H
# define FT_PING_H
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <netinet/ip_icmp.h>
# include <sysexits.h>

# define V_FLAG (1 << 0)	// v(verbose) flag
# define Q_FLAG (1 << 1)	// ?(usage) flag
# define F_FLAG (1 << 2)	// f(flood) flag
# define L_FLAG (1 << 3)	// l(preload) flag
# define N_FLAG (1 << 4)	// n(numeric preload) flag
# define W_FLAG (1 << 5)	// w(timeout N) flag
# define CW_FLAG (1 << 6)	// W(linger N)
# define INVALID_F (1 << 7)	// Invalid option flag
# define MAX_WAIT 10

typedef struct s_options			t_options;

struct s_options
{
	u_char flags;
	u_char invalid_option;
};


struct s_ping
{
	;
};

t_options parseargs(int argc, char **argv);

#endif
