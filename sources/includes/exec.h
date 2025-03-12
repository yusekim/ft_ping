#ifndef EXEC_H
# define EXEC_H
# include "structures.h"

int				exec_ping(t_options *options);
t_ping_info		*build_info(t_options *options, int idx);
int 			sendpacket(t_options *options, t_ping_info *info, struct timespec *now, uint16_t *seqnum);

void			*info_free(t_ping_info *info, int is_perror);
struct addrinfo	*getdestinfo(char *hostname);
int				ping_exit(t_options *options, t_ping_info *info, int code);
char			*build_preload(int num, uint16_t id);
struct timespec	*build_preload_time(int num);
double 			timespec_diff(struct timespec start, struct timespec end);

#endif
