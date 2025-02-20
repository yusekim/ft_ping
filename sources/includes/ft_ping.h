#ifndef FT_PING_H
# define FT_PING_H
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <netinet/ip_icmp.h>
# include <sysexits.h>
# include <string.h>
# include <time.h>
# include "structures.h"

# define V_FLAG (1 << 0)	// v(verbose) flag
# define Q_FLAG (1 << 1)	// ?(usage) flag
# define F_FLAG (1 << 2)	// f(flood) flag
# define L_FLAG (1 << 3)	// l(preload N) flag
# define N_FLAG (1 << 4)	// n(numeric) flag
# define W_FLAG (1 << 5)	// w(timeout N) flag
# define CW_FLAG (1 << 6)	// W(linger N)
# define INVALID_F (1 << 7)	// Invalid option flag
# define MAX_WAIT 10		// default maximum seconds of wait to get reply msg
# define INTERVAL 1			// interval wait seconds between packet send in normal mode

#endif
