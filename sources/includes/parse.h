#ifndef PARSE_H
# define PARSE_H
# include "structures.h"

# define PING_USAGE "Usage: ping [OPTION...] HOST ...\n" \
"\n" \
" Options valid for all request types:\n" \
"  -v, verbose                verbose output\n" \
"  -n, numeric                do not resolve host addresses\n" \
"  -w, timeout=N              stop after N seconds\n" \
"  -W, linger=N               number of seconds to wait for response\n" \
"\n" \
" Options valid for echo requests:\n" \
"  -f, flood                  flood ping (root only)\n" \
"  -l, preload=NUMBER         send NUMBER packets as fast as possible before\n" \
"                             falling into normal mode of behavior (root only)\n" \
"  -?                         give this help list\n"

# define INVALID_MSG "ping: invalid"

# define INVALID_ARG_HELP_MSG "Try 'ping -?' for more information.\n"

t_ping_info		*parseargs(int argc, char **argv);
void			handle_options(t_ping_info *info, char *arg, char ***argv);
int				get_opt_val(t_ping_info *info, char flag, char ***argv);


void			print_ping_info(t_ping_info *info);

#endif
