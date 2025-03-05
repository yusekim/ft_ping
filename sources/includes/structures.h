#ifndef STRUCTURES_H
# define STRUCTURES_H
# include <stdio.h>
# include <stdlib.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/ip_icmp.h>
# include <netdb.h>
# include <sysexits.h>
# include <string.h>
# include <time.h>
# include "strs.h"
# include "utils.h"

# define MAX_LEVEL 9
# define PACKET_SIZE 64		// packet size(64bytes)

typedef struct s_options				t_options;
typedef struct s_ping_info				t_ping_info;
typedef struct s_slist					t_slist;

struct s_options
{
	unsigned char	flags;
	char			**hosts;
	int				hosts_num;
	int				preload_num;
	int				packets_count;
	int				timeout;
	uint8_t			ttl_val;
};

struct s_ping_info
{
	int					sockfd;
	struct addrinfo		*dest_info;			// needs to free
	t_slist				*packets;
	char				*pre_packets;
	int					dup_sum;
	struct s_ping_info	*next;
};

struct s_slist
{
	int						val;
	int						level;
	int						dup_cnt;
	char					packet[64];
	struct s_slist			*level_ptrs[MAX_LEVEL + 1];
};

t_slist			*slist_search(t_slist *head, int key);
void			slist_push_back(t_slist **head, int key);
void			slist_free(t_slist *head);
void			slist_delete(t_slist **head, int key);
int				randomlevel();

#endif
