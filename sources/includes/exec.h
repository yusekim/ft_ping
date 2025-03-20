#ifndef EXEC_H
# define EXEC_H
# include "structures.h"
# include <errno.h>
# include <math.h>

# define HDR_DUMP_MSG "\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst	Data\n"
# define ROUND_TRIP_MSG "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n"

int				exec_ping(t_options *options);
t_ping_info		*build_info(t_options *options, int idx);
int 			sendpacket(int idx, t_options *options, t_ping_info *info, struct timespec *prev, uint16_t *seqnum, t_stat *stat);
int				check_ping_expired(t_options *options, t_ping_info *info, struct timespec *now);
void			print_stats(t_slist *head, t_stat *stat);

void			*info_free(t_ping_info *info, int is_perror);
struct addrinfo	*getdestinfo(char *hostname);
int				ping_exit(t_options *options, t_ping_info *info, int code);
void			print_verbose(struct iphdr *int_hdr, struct icmphdr *sent_icmp);
double 			timespec_diff(struct timespec start, struct timespec end);
void			set_stat(t_stat *stat);

#endif
