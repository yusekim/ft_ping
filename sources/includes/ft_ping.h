#ifndef FT_PING_H
# define FT_PING_H
# include <stdlib.h>
# include <unistd.h>
# include "structures.h"

# define INVALID_F (1 << 0)	// Invalid option flag
# define Q_FLAG (1 << 1)	// ?(usage) flag
# define V_FLAG (1 << 2)	// v(verbose) flag
# define C_FLAG (1 << 3)	// c(count N) flag
# define CW_FLAG (1 << 4)	// W(linger N) flag : Maximum number of seconds n to wait for a response.
# define W_FLAG (1 << 5)	// w(timeout N) flag : Stop after n seconds.
# define TTL_FLAG (1 << 6)	// ttl=N(time to live N) flag
# define MAX_WAIT 10		// default maximum seconds of wait to get reply msg
# define INTERVAL 1			// interval wait seconds between packet send in normal mode

#endif
