#ifndef FT_PING_H
# define FT_PING_H
# include <stdlib.h>
# include <unistd.h>
# include "structures.h"
# include "utils.h"

# define V_FLAG (1 << 0)	// v(verbose) flag
# define Q_FLAG (1 << 1)	// ?(usage) flag
# define C_FLAG (1 << 2)	// c(count N) flag
# define L_FLAG (1 << 3)	// l(preload N) flag
# define N_FLAG (1 << 4)	// n(numeric) flag
# define W_FLAG (1 << 5)	// w(timeout N) flag
# define TTL_FLAG (1 << 6)	// ttl=N(time to live N) flag
# define INVALID_F (1 << 7)	// Invalid option flag
# define MAX_WAIT 10		// default maximum seconds of wait to get reply msg
# define INTERVAL 1			// interval wait seconds between packet send in normal mode

#endif
