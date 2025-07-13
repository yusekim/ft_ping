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
typedef struct s_options		t_options;
typedef struct s_ping_info		t_ping_info;
typedef struct s_packet_data	t_packet_data;
typedef struct s_list			t_list;
typedef struct s_stat			t_stat;

struct s_options
{
	unsigned char				flags;
	char						**hosts;
	int							hosts_num;
	int							sockfd;
	int							timeout;
	int							linger;
	uint8_t						ttl_val;
	uint16_t					packets_count;
	uint16_t					id;
};

struct s_ping_info
{
	struct addrinfo				*dest_info;
	t_list						*packets;
	uint16_t					count;
};

struct s_packet_data
{
	int							val;			// same as packet seq number
	int							is_received;
	char						packet[PACKET_SIZE];
	struct timespec				senttime;
	double						time_taken_ms;
};

struct s_list
{
	void						*data;
	struct s_list				*next;
};

struct s_stat
{
	int							sent;
	int							recved;
	int							dup_count;
	double						min;
	double						sum;
	double						max;
};

t_list			*list_search(t_list *head, int key);
t_list			*list_push_back(t_list **head, int key);
void			list_delete(t_list **head, int key);
void			list_free(t_list *head);

#endif
