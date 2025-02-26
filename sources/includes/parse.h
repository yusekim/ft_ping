#ifndef PARSE_H
# define PARSE_H
# include "structures.h"

# define PING_USAGE "Usage: ping [OPTION...] HOST ...\n" \
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

t_ping_info		*parseargs(int argc, char **argv, t_options *options);
t_ping_info		*build_info(t_options *options, int len);
void			handle_options(t_options *t_options, char *arg, char ***argv);
int				get_opt_val(t_options *options, char flag, char ***argv);
int				get_ttl_val(t_options *options, char *flag, int *idx);

void			*info_free(t_ping_info *info, int is_perror);
char			*build_preload(int num, uint16_t id);
char			*is_ascii_number(char *str);

void			print_ping_info(t_ping_info *info, t_options *options);
#endif
