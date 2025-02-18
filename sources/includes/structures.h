#ifndef STRUCTURES_H
# define STRUCTURES_H
# include "skip_list.h"

typedef struct s_options			t_options;
typedef struct s_ping_info				t_ping_info;

struct s_options
{
	unsigned char flags;
	char	**hosts;
};

struct s_ping_info
{
	int l_flag_val;
	int w_flag_val;
	int W_flag_val;
	t_slist *packets;
};

#endif
