#include "list.h"
#include <stdio.h>

list_t *
list_init(void)
{
	list_t *newlist;

	newlist = (list_t *)malloc(sizeof(list_t));

	newlist->head = NULL;
	newlist->tail = NULL;

	return newlist;
}

void
list_add(list_t *list, void *entity)
{
	link_t *newlink = (link_t *)malloc(sizeof(link_t));
	newlink->next = NULL;
	newlink->prev = NULL;
	newlink->item = entity;

	if(list->head == NULL) {
		list->head = newlink;
		list->tail = newlink;
	} else {
		newlink->prev = list->tail;
		list->tail->next = newlink;
		list->tail = newlink;
	}
}

void
list_del_item(list_t *list, void *item)
{
	link_t *templink;

	if(list == NULL)
		return;

	templink = list->head;
	while(templink != NULL && templink->item != item)
		templink = templink->next;

	if(templink == NULL)
		return;

	if(templink == list->tail && templink == list->head) {
		list->head = NULL;
		list->tail = NULL;
	} else if(templink == list->tail) {
		templink->prev->next = templink->next;
	} else if(templink == list->head) {
		templink->next->prev = templink->prev;
	} else {
		templink->prev->next = templink->next;
		templink->next->prev = templink->prev;
	}

	free(templink);
}

void
list_del(list_t *list)
{
	link_t *templink;

	while(list->head != NULL)
	{
		templink = list->head;
		list->head = list->head->next;
		if (templink->item != NULL)
			free(templink->item);
		if (templink != NULL)
			free(templink);
	}

	free(list);
}

void
list_tail_to_head(list_t *list)
{
	if(list->tail == list->head)
		return;
	else {
		list->tail->next = list->head;
		list->head->prev = list->tail;
		list->head = list->tail;
		list->tail = list->tail->prev;
		list->head->prev = NULL;
		list->tail->next = NULL;
	}
}

void
list_head_to_tail(list_t *list)
{
	if(list->tail == list->head)
		return;
	else {
		list->head->prev = list->tail;
		list->tail->next = list->head;
		list->tail = list->head;
		list->head = list->head->next;
		list->tail->next = NULL;
		list->head->prev = NULL;
	}
}
