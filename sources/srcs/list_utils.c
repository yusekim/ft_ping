#include "structures.h"

t_list *list_search(t_list *head, int key)
{
	t_list *cur_node = head;
	while (cur_node)
	{
		t_packet_data *cur_data = (t_packet_data *)(cur_node->data);
		if (cur_data && cur_data->val == key)
			break;
		cur_node = cur_node->next;
	}
	return cur_node;
}

t_list *list_push_back(t_list **head, int key)
{
	if (head == NULL)
		return NULL;
	t_list *new_node = calloc(1, sizeof(t_list));
	if (!new_node)
		return NULL;
	t_packet_data *new_data = calloc(1, sizeof(t_packet_data));
	if (!new_data)
	{
		free(new_node);
		return NULL;
	}
	new_node->data = (void *)new_data;
	new_data->val = key;
	clock_gettime(CLOCK_MONOTONIC, &new_data->senttime);
	if (*head == NULL)
		*head = new_node;
	else
	{
		t_list *node = *head;
		while (node->next)
			node = node->next;
		node->next = new_node;
	}
	return new_node;
}

void list_delete(t_list **head, int key)
{
	if (!head || !*head)
		return ;
	t_list *cur_node = *head;
	t_list *prev_node = NULL;
	while (cur_node)
	{
		t_packet_data *data = (t_packet_data *)(cur_node->data);
		if (data->val == key)
		{
			if (prev_node)
				prev_node->next = cur_node->next;
			else
				(*head) = cur_node->next;
			free(cur_node->data);
			free(cur_node);
			break;
		}
		prev_node = cur_node;
		cur_node = cur_node->next;
	}
}

void list_free(t_list *head)
{
	t_list *temp;
	while (head)
	{
		temp = head->next;
		free(head->data);
		free(head);
		head = temp;
	}
}
