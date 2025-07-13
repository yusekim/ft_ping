#ifndef PARSE_H
# define PARSE_H
# include "structures.h"

# define PING_USAGE "Usage: ft_ping [OPTION...] HOST ...\n" \
"\n" \
"  -v, verbose                verbose output\n" \
"  -c n                       Stop after sending and receiving answers to a total of n packets.\n" \
"  -W n                       Maximum number of seconds n to wait for a response.\n" \
"  -w n                       Stop after n seconds\n" \
"  --ttl=n                    Specify n as time-to-live\n" \
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
