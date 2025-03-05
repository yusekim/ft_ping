#ifndef EXEC_H
# define EXEC_H
# include "structures.h"

int				exec_ping(t_options *options);
t_ping_info		*build_info(t_options *options, int idx);

void			*info_free(t_ping_info *info, int is_perror);
struct addrinfo	*getdestinfo(char *hostname);
char			*build_preload(int num, uint16_t id);

#endif
