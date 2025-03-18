#ifndef STRUCTURES_H
# define STRUCTURES_H
# include <stdio.h>
# include <stdlib.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netinet/ip_icmp.h>
# include <netdb.h>
# include <sysexits.h>
# include <string.h>
# include <time.h>
# include "strs.h"


# define MAX_LEVEL 9
# define PACKET_SIZE 64
# define IPHDR_SIZE 20
typedef struct s_options				t_options;
typedef struct s_ping_info				t_ping_info;
typedef struct s_slist					t_slist;
typedef struct s_stat					t_stat;

struct s_options
{
	unsigned char	flags;
	char			**hosts;
	int				hosts_num;
	int				sockfd;
	uint16_t		timeout;
	uint8_t			ttl_val;
	uint16_t		packets_count;
	uint16_t		linger;
	uint16_t		id;
};

struct s_ping_info
{
	struct addrinfo		*dest_info;			// needs to free
	t_slist				*packets;
	uint16_t			count;
};

struct s_slist
{
	int						val;			// seq number
	int						level;
	int						is_received;
	char					packet[64];
	struct timespec			senttime;
	double					time_taken_ms;
	struct s_slist			*level_ptrs[MAX_LEVEL + 1];
};

struct s_stat
{
	int					sent;
	int					recved;
	int					dup_count;
	double				min;
	double				sum;
	double				max;
};

t_slist			*slist_search(t_slist *head, int key);
t_slist			*slist_push_back(t_slist **head, int key);
void			slist_free(t_slist *head);
void			slist_delete(t_slist **head, int key);
int				randomlevel();

#endif
