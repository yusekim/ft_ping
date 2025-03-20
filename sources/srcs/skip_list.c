#include "structures.h"

static t_slist	*nodes_to_null[MAX_LEVEL + 1];		// 각 level에서 NULL 을 가르키는 last nodes pointer

t_slist *slist_search(t_slist *head, int key)
{
	if (head == NULL)
		return NULL;
	t_slist *current = head;
	int start_level = head->level;
	for (int i = start_level; i >= 0; i--)
	{
		while (current->level_ptrs[i] != NULL && \
				current->level_ptrs[i] != nodes_to_null[i] && \
				current->level_ptrs[i]->val < key)
		{
			current = current->level_ptrs[i];
		}

	}
	current = current->level_ptrs[0];
	if (current != NULL && current->val == key)
		return current;
	else
		return NULL;
}

void slist_delete(t_slist **head, int key)
{
	if (!head || !*head)
		return ;
	t_slist *prev_nodes[MAX_LEVEL + 1] = {0};
	t_slist *current = *head;

	for (int level = MAX_LEVEL; level >= 0; level--)
	{
		while (current->level_ptrs[level] && current->level_ptrs[level]->val < key)
			current = current->level_ptrs[level];
		prev_nodes[level] = current;
	}
	t_slist *target = current->level_ptrs[0];
	if (!target || target->val != key)
	{
		// TODO: handle the ping (DUP!) situation
		return;
	}
	for (int level = 0; level <= MAX_LEVEL; level++)
	{
		if (prev_nodes[level]->level_ptrs[level] == target)
			prev_nodes[level]->level_ptrs[level] = target->level_ptrs[level];
	}
	free(target);
}

t_slist *slist_push_back(t_slist **head, int key)
{
	if (head == NULL)
		return NULL;

	if (*head == NULL)
	{
		t_slist *new_head = malloc(sizeof(t_slist));
		if (!new_head)
			return NULL;
		new_head->val = -1;
		new_head->level = MAX_LEVEL;
		for (int i = 0; i <= MAX_LEVEL; i++)
		{
			new_head->level_ptrs[i] = NULL;
			nodes_to_null[i] = new_head;
		}
		*head = new_head;
	}

	t_slist *new_node = calloc(1, sizeof(t_slist));
	if (!new_node)
		return NULL;

	new_node->val = key;
	new_node->level = randomlevel();
	for (int level = new_node->level; level >= 0; level--)
	{
		nodes_to_null[level]->level_ptrs[level] = new_node;
		nodes_to_null[level] = new_node;
	}
	clock_gettime(CLOCK_MONOTONIC, &new_node->senttime);

	return new_node;
}

void slist_free(t_slist *head)
{
	t_slist *temp;
	while (head)
	{
		temp = head->level_ptrs[0];
		free(head);
		head = temp;
	}
}

int randomlevel()
{
	double p = 0.5;
	int level = 0;
	while (level < MAX_LEVEL)
	{
		if (drand48() < p)
			level++;
		else
			break;
	}
	return level;
}
