#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

typedef struct link_type
{
   struct link_type *next;  /* Next link in the list   */
   struct link_type *prev;  /* Previous link in the list   */
   void             *item;  /* Item owned by this link */
}  link_t;

typedef struct list_type
{
   link_t  *head;           /* First link in the list */
   link_t  *tail;           /* Last link in the list  */
}  list_t;

/* malloc a new list header and initialize it */
list_t *list_init(void);

/* Add an element to the end of a list */
void list_add(list_t *list, void *item);

/* Delete an element from the list */
void list_del_item(list_t *list, void *entity);

/* Delete all of the list control structures and */
/* the entities they control                     */
void list_del(list_t *list);

/* move tail to the head */
void list_tail_to_head(list_t *list);

/* move head to the tail */
void list_head_to_tail(list_t *list);

#endif /* !_LIST_H_ */