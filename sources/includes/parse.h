#ifndef PARSE_H
# define PARSE_H
# include "structures.h"

# define PING_USAGE "Usage: ft_ping [OPTION...] HOST ...\n" \
"\n" \
" Options valid for all request types:\n" \
"  -v, verbose                verbose output\n" \
"  -n, numeric                do not resolve host addresses\n" \
"  -w, timeout=N              stop after N seconds\n" \
"      --ttl=N                specify N as time-to-live\n" \
"\n" \
" Options valid for echo requests:\n" \
"  -f, flood                  flood ping (root only)\n" \
"  -l, preload=NUMBER         send NUMBER packets as fast as possible before\n" \
"                             falling into normal mode of behavior (root only)\n" \
"  -?                         give this help list\n"

# define INVALID_MSG "ft_ping: invalid"

# define INVALID_ARG_HELP_MSG "Try 'ping -?' for more information.\n"

int				getoptions(char **argv, t_options *options);
void			handle_options(t_options *t_options, char *arg, char ***argv);
int				get_opt_val(t_options *options, char flag, char ***argv);
void			get_ttl_val(t_options *options, char *flag);

char			*is_ascii_number(char *str);
void			print_option_info(t_options *options);

#endif
