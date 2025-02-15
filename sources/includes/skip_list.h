#ifndef SKIP_LIST_H
# define SKIP_LIST_H
# include <stdlib.h>
# include <sys/time.h>

# define MAX_LEVEL 9

typedef struct s_slist		t_slist;

struct s_slist
{
	int						val;
	int						level;
	struct s_slist			*level_ptrs[MAX_LEVEL + 1];
};

t_slist			*slist_search(t_slist *head, int key);
void			slist_push_back(t_slist **head, int key);
void			slist_delete(t_slist **head, int key);
int				randomlevel();

#endif
