/**
  ******************************************************************************
  * @file        : my_list.h
  * @author      : 
  * @version     : V1.0
  * @date        : 
  * @brief       : Double linked list implementation
  * @attention   : None
  ******************************************************************************
  */

#ifndef __MY_LIST_H
#define __MY_LIST_H

#include <stdint.h>
#include <stddef.h>
#include "types.h"

/**
 * @brief  Double list structure
 */
struct list_node {
    struct list_node *next, *prev;
};
typedef struct list_node list_t;

/**
 * @brief  Single List structure
 */
struct slist_node {
    struct slist_node *next;
};
typedef struct slist_node slist_t;

/**
 * @brief  Single List structure
 */
struct hlist_head {
	struct hlist_node *first;
};

/**
 * @brief  Single List structure
 */
struct hlist_node {
	struct hlist_node *next, **pprev;
};

/**
 * @brief  Define a LIST_HEAD macro for convenience
 * @param  name: Name of the list head
 */
#define LIST_HEAD(name) \
	list_t name = { &(name), &(name) }

/**
 * @brief  Initialize a list_node structure
 * @param  node: list_node structure to be initialized
 * @retval None
 * 
 * Initializes the list_node to point to itself. If it is a list header,
 * the result is an empty list.
 */
static inline void list_node_init(list_t *node)
{
    node->next = node;
    node->prev = node;
}

/**
 * @brief  Insert a new_node list_node entry between two known consecutive entries
 * @param  new_node: New node to insert
 * @param  prev: Previous node in list
 * @param  next: Next node in list
 * @retval None
 */
static inline void __list_add(list_t *new_node, list_t *prev, list_t *next)
{
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

/**
 * @brief  Add the node1 after the node2
 * @param  node1: list_node structure to be added
 * @param  node2: list_node structure to add it after
 * @retval None
 * 
 *  head → A → B     ->     head → new → A → B
 *  ↑__________↓             ↑_______________↓
 */
static inline void list_add(list_t *new_node, list_t *node)
{
    __list_add(new_node, node, node->next);
}

/**
 * @brief  Add the node1 before the node2
 * @param  node1: New node entry to be added
 * @param  node2: list_node structure to add it before
 * @retval None
 * 
 * 
 *  head → A → B     ->     head → A → B → new
 *  ↑__________↓             ↑_______________↓
 */
static inline void list_add_tail(list_t *new_node, list_t *node)
{
    __list_add(new_node, node->prev, node);
}

/**
 * @brief  Delete a list entry by making the prev/next entries point to each other
 * @param  prev: Previous node in list
 * @param  next: Next node in list
 * @retval None
 * 
 * This is only for internal list manipulation where we know
 * the prev/next entries already (is not NULL)!
 */
static inline void __list_del(list_t *prev, list_t *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void __list_del_entry(list_t *node)
{
	__list_del(node->prev, node->next);
}

/**
 * @brief  Delete entry from list
 * @param  node: The element to delete from the list
 * 
 * Note: list_empty() on node does not return true after this, the node is
 * in an undefined state.
 */
static inline void list_del(list_t *node)
{
    __list_del(node->prev, node->next);
    node->next = (void*)0xFFFFFFFF;
    node->prev = (void*)0xFFFFFFFF;
}

static inline void list_del_init(list_t *node)
{
    __list_del_entry(node);
    list_node_init(node);
}

/**
 * @brief  Replace old node by new one
 * @param  old_node: The element to be replaced
 * @param  new_node: The new element to insert
 * @retval None
 * 
 * If old_node was empty, it will be overwritten.
 */
static inline void list_replace(list_t *old_node, list_t *new_node)
{
	new_node->next = old_node->next;
	new_node->next->prev = new_node;
	new_node->prev = old_node->prev;
	new_node->prev->next = new_node;
}

static inline void list_replace_init(list_t *old_node, list_t *new_node)
{
	list_replace(old_node, new_node);
	list_node_init(old_node);
}

/**
 * @brief  Delete from one list and add as another's head
 * @param  node1: The entry to move
 * @param  node2: The head that will precede our entry
 * @retval None
 */
static inline void list_move(list_t *node1, list_t *node2)
{
	__list_del_entry(node1);
	list_add(node1, node2);
}

/**
 * @brief  Move a list before the specified head position
 * @param  node1: The entry to move
 * @param  node2: The head that will follow our entry
 * @retval None
 */
static inline void list_move_tail(list_t *node1, list_t *node2)
{
	__list_del_entry(node1);
	list_add_tail(node1, node2);
}

/**
 * @brief  Test whether node is the last entry in list head
 * @param  node: The entry to test
 * @param  head: The head of the list
 * @retval 1 if true, 0 if false
 */
static inline int list_is_last(const list_t *node, const list_t *head)
{
	return node->next == head;
}

/**
 * @brief  Test whether a list is empty
 * @param  head: The list to test
 * @retval 1 if empty, 0 if not empty
 */
static inline int list_empty(const list_t *head)
{
	return head->next == head;
}


static inline int list_empty_careful(const list_t *head)
{
	list_t *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * @brief  Test whether a list has just one entry
 * @param  head: The list to test
 * @retval 1 if singular, 0 if not singular
 */
static inline int list_is_singular(const list_t *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

/**
 * @brief  Get the struct for this entry
 * @param  ptr: The &list_t pointer
 * @param  type: The type of the struct this is embedded in
 * @param  member: The name of the list_node within the struct
 * @return The struct for this entry
 */
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * @brief  Get the first element from a list
 * @param  ptr: The list head to take the element from
 * @param  type: The type of the struct this is embedded in
 * @param  member: The name of the list_node within the struct
 * @retval The first element of the list
 * 
 * Note that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * @brief  Get the last element from a list
 * @param  ptr: The list head to take the element from
 * @param  type: The type of the struct this is embedded in
 * @param  member: The name of the list_node within the struct
 * @retval The last element of the list
 * 
 * Note that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

/**
 * @brief  Get the first element from a list
 * @param  ptr: The list head to take the element from
 * @param  type: The type of the struct this is embedded in
 * @param  member: The name of the list_node within the struct
 * @retval The first element of the list, or NULL if empty
 * 
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) \
	(!list_empty(ptr) ? list_first_entry(ptr, type, member) : NULL)
    
#define list_last_entry_or_null(ptr, type, member) \
	(!list_empty(ptr) ? list_last_entry(ptr, type, member) : NULL)

/**
 * @brief  Get the next element in list
 * @param  pos: The type * to cursor
 * @param  member: The name of the list_node within the struct
 * @retval The next element in list
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * @brief  Get the previous element in list
 * @param  pos: The type * to cursor
 * @param  member: The name of the list_node within the struct
 * @retval The previous element in list
 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * @brief  Iterate over a list
 * @param  pos: The &struct list_head to use as a loop cursor
 * @param  head: The head for your list
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * @brief  Iterate over a list backwards
 * @param  pos: The &struct list_head to use as a loop cursor
 * @param  head: The head for your list
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * @brief  Iterate over a list safe against removal of list entry
 * @param  pos: The &struct list_head to use as a loop cursor
 * @param  n: Another &struct list_head to use as temporary storage
 * @param  head: The head for your list
 */
#define list_for_each_safe(pos, n, head)                           \
	for (pos = (head)->next, n = pos->next; pos != (head);          \
		pos = n, n = pos->next)

/**
 * @brief  Iterate over a list backwards safe against removal of list entry
 * @param  pos: The &struct list_head to use as a loop cursor
 * @param  n: Another &struct list_head to use as temporary storage
 * @param  head: The head for your list
 */
#define list_for_each_prev_safe(pos, n, head)                      \
	for (pos = (head)->prev, n = pos->prev;                         \
	     pos != (head);                                             \
	     pos = n, n = pos->prev)

/**
 * @brief  Iterate over list of given type
 * @param  pos: The type * to use as a loop cursor
 * @param  head: The head for your list
 * @param  member: The name of the list_head within the struct
 */
#define list_for_each_entry(pos, head, member)				        \
	for (pos = list_first_entry(head, typeof(*pos), member);	    \
	     &pos->member != (head);					                \
	     pos = list_next_entry(pos, member))

/**
 * @brief  Iterate backwards over list of given type
 * @param  pos: The type * to use as a loop cursor
 * @param  head: The head for your list
 * @param  member: The name of the list_head within the struct
 */
#define list_for_each_entry_reverse(pos, head, member)			    \
	for (pos = list_last_entry(head, typeof(*pos), member);		\
	     &pos->member != (head); 					                \
	     pos = list_prev_entry(pos, member))

/**
 * @brief  Prepare a pos entry for use in list_for_each_entry_continue()
 * @param  pos: The type * to use as a start point
 * @param  head: The head of the list
 * @param  member: The name of the list_head within the struct
 * @return The prepared entry
 */
#define list_prepare_entry(pos, head, member) \
	((pos) ? : list_entry(head, typeof(*pos), member))

/**
 * @brief  Continue iteration over list of given type
 * @param  pos: The type * to use as a loop cursor
 * @param  head: The head for your list
 * @param  member: The name of the list_head within the struct
 * 
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, head, member) 		    \
	for (pos = list_next_entry(pos, member);			            \
	     &pos->member != (head);					                \
	     pos = list_next_entry(pos, member))

/**
 * @brief  Continue iteration over list of given type backwards
 * @param  pos: The type * to use as a loop cursor
 * @param  head: The head for your list
 * @param  member: The name of the list_head within the struct
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(pos, head, member)	\
	for (pos = list_prev_entry(pos, member);			            \
	     &pos->member != (head);					                \
	     pos = list_prev_entry(pos, member))

/**
 * @brief  Iterate over list of given type from the current point
 * @param  pos: The type * to use as a loop cursor
 * @param  head: The head for your list
 * @param  member: The name of the list_head within the struct
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member) 			    \
	for (; &pos->member != (head);					                \
	     pos = list_next_entry(pos, member))

/**
 * @brief  Iterate over list of given type safe against removal of list entry
 * @param  pos: The type * to use as a loop cursor
 * @param  n: Another type * to use as temporary storage
 * @param  head: The head for your list
 * @param  member: The name of the list_head within the struct
 */
#define list_for_each_entry_safe(pos, n, head, member)			    \
	for (pos = list_first_entry(head, typeof(*pos), member),	    \
		n = list_next_entry(pos, member);			                \
	     &pos->member != (head); 					                \
	     pos = n, n = list_next_entry(n, member))

/**
 * @brief  Continue list iteration safe against removal
 * @param  pos: The type * to use as a loop cursor
 * @param  n: Another type * to use as temporary storage
 * @param  head: The head for your list
 * @param  member: The name of the list_head within the struct
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member) 	\
	for (pos = list_next_entry(pos, member), 				        \
		n = list_next_entry(pos, member);				            \
	     &pos->member != (head);						            \
	     pos = n, n = list_next_entry(n, member))

/**
 * @brief  Iterate over list from current point safe against removal
 * @param  pos: The type * to use as a loop cursor
 * @param  n: Another type * to use as temporary storage
 * @param  head: The head for your list
 * @param  member: The name of the list_head within the struct
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) 		\
	for (n = list_next_entry(pos, member);					        \
	     &pos->member != (head);						            \
	     pos = n, n = list_next_entry(n, member))

/**
 * @brief  Iterate backwards over list safe against removal
 * @param  pos: The type * to use as a loop cursor
 * @param  n: Another type * to use as temporary storage
 * @param  head: The head for your list
 * @param  member: The name of the list_head within the struct
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = list_last_entry(head, typeof(*pos), member),		\
		n = list_prev_entry(pos, member);			                \
	     &pos->member != (head); 					                \
	     pos = n, n = list_prev_entry(n, member))

/**
 * @brief  Reset a stale list_for_each_entry_safe loop
 * @param  pos: The loop cursor used in the list_for_each_entry_safe loop
 * @param  n: Temporary storage used in list_for_each_entry_safe
 * @param  member: The name of the list_head within the struct
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define list_safe_reset_next(pos, n, member) \
	n = list_next_entry(pos, member)

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = {  .first = NULL }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
static inline void INIT_HLIST_NODE(struct hlist_node *h)
{
	h->next = NULL;
	h->pprev = NULL;
}

/**
 * hlist_unhashed - Has node been removed from list and reinitialized?
 * @h: Node to be checked
 *
 * Not that not all removal functions will leave a node in unhashed
 * state.  For example, hlist_nulls_del_init_rcu() does leave the
 * node in unhashed state, but hlist_nulls_del() does not.
 */
static inline int hlist_unhashed(const struct hlist_node *h)
{
	return !h->pprev;
}

///**
// * hlist_unhashed_lockless - Version of hlist_unhashed for lockless use
// * @h: Node to be checked
// *
// * This variant of hlist_unhashed() must be used in lockless contexts
// * to avoid potential load-tearing.  The READ_ONCE() is paired with the
// * various WRITE_ONCE() in hlist helpers that are defined below.
// */
//static inline int hlist_unhashed_lockless(const struct hlist_node *h)
//{
//	return !READ_ONCE(h->pprev);
//}

///**
// * hlist_empty - Is the specified hlist_head structure an empty hlist?
// * @h: Structure to check.
// */
//static inline int hlist_empty(const struct hlist_head *h)
//{
//	return !READ_ONCE(h->first);
//}

//static inline void __hlist_del(struct hlist_node *n)
//{
//	struct hlist_node *next = n->next;
//	struct hlist_node **pprev = n->pprev;

//	WRITE_ONCE(*pprev, next);
//	if (next)
//		WRITE_ONCE(next->pprev, pprev);
//}

///**
// * hlist_del - Delete the specified hlist_node from its list
// * @n: Node to delete.
// *
// * Note that this function leaves the node in hashed state.  Use
// * hlist_del_init() or similar instead to unhash @n.
// */
//static inline void hlist_del(struct hlist_node *n)
//{
//	__hlist_del(n);
//	n->next = LIST_POISON1;
//	n->pprev = LIST_POISON2;
//}

///**
// * hlist_del_init - Delete the specified hlist_node from its list and initialize
// * @n: Node to delete.
// *
// * Note that this function leaves the node in unhashed state.
// */
//static inline void hlist_del_init(struct hlist_node *n)
//{
//	if (!hlist_unhashed(n)) {
//		__hlist_del(n);
//		INIT_HLIST_NODE(n);
//	}
//}

///**
// * hlist_add_head - add a new entry at the beginning of the hlist
// * @n: new entry to be added
// * @h: hlist head to add it after
// *
// * Insert a new entry after the specified head.
// * This is good for implementing stacks.
// */
//static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
//{
//	struct hlist_node *first = h->first;
//	WRITE_ONCE(n->next, first);
//	if (first)
//		WRITE_ONCE(first->pprev, &n->next);
//	WRITE_ONCE(h->first, n);
//	WRITE_ONCE(n->pprev, &h->first);
//}

///**
// * hlist_add_before - add a new entry before the one specified
// * @n: new entry to be added
// * @next: hlist node to add it before, which must be non-NULL
// */
//static inline void hlist_add_before(struct hlist_node *n,
//				    struct hlist_node *next)
//{
//	WRITE_ONCE(n->pprev, next->pprev);
//	WRITE_ONCE(n->next, next);
//	WRITE_ONCE(next->pprev, &n->next);
//	WRITE_ONCE(*(n->pprev), n);
//}

///**
// * hlist_add_behind - add a new entry after the one specified
// * @n: new entry to be added
// * @prev: hlist node to add it after, which must be non-NULL
// */
//static inline void hlist_add_behind(struct hlist_node *n,
//				    struct hlist_node *prev)
//{
//	WRITE_ONCE(n->next, prev->next);
//	WRITE_ONCE(prev->next, n);
//	WRITE_ONCE(n->pprev, &prev->next);

//	if (n->next)
//		WRITE_ONCE(n->next->pprev, &n->next);
//}

///**
// * hlist_add_fake - create a fake hlist consisting of a single headless node
// * @n: Node to make a fake list out of
// *
// * This makes @n appear to be its own predecessor on a headless hlist.
// * The point of this is to allow things like hlist_del() to work correctly
// * in cases where there is no list.
// */
//static inline void hlist_add_fake(struct hlist_node *n)
//{
//	n->pprev = &n->next;
//}

///**
// * hlist_fake: Is this node a fake hlist?
// * @h: Node to check for being a self-referential fake hlist.
// */
//static inline bool hlist_fake(struct hlist_node *h)
//{
//	return h->pprev == &h->next;
//}

///**
// * hlist_is_singular_node - is node the only element of the specified hlist?
// * @n: Node to check for singularity.
// * @h: Header for potentially singular list.
// *
// * Check whether the node is the only node of the head without
// * accessing head, thus avoiding unnecessary cache misses.
// */
//static inline bool
//hlist_is_singular_node(struct hlist_node *n, struct hlist_head *h)
//{
//	return !n->next && n->pprev == &h->first;
//}

///**
// * hlist_move_list - Move an hlist
// * @old: hlist_head for old list.
// * @new: hlist_head for new list.
// *
// * Move a list from one list head to another. Fixup the pprev
// * reference of the first entry if it exists.
// */
//static inline void hlist_move_list(struct hlist_head *old,
//				   struct hlist_head *new)
//{
//	new->first = old->first;
//	if (new->first)
//		new->first->pprev = &new->first;
//	old->first = NULL;
//}

///**
// * hlist_splice_init() - move all entries from one list to another
// * @from: hlist_head from which entries will be moved
// * @last: last entry on the @from list
// * @to:   hlist_head to which entries will be moved
// *
// * @to can be empty, @from must contain at least @last.
// */
//static inline void hlist_splice_init(struct hlist_head *from,
//				     struct hlist_node *last,
//				     struct hlist_head *to)
//{
//	if (to->first)
//		to->first->pprev = &last->next;
//	last->next = to->first;
//	to->first = from->first;
//	from->first->pprev = &to->first;
//	from->first = NULL;
//}

//#define hlist_entry(ptr, type, member) container_of(ptr,type,member)

//#define hlist_for_each(pos, head) \
//	for (pos = (head)->first; pos ; pos = pos->next)

//#define hlist_for_each_safe(pos, n, head) \
//	for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
//	     pos = n)

//#define hlist_entry_safe(ptr, type, member) \
//	({ typeof(ptr) ____ptr = (ptr); \
//	   ____ptr ? hlist_entry(____ptr, type, member) : NULL; \
//	})

///**
// * hlist_for_each_entry	- iterate over list of given type
// * @pos:	the type * to use as a loop cursor.
// * @head:	the head for your list.
// * @member:	the name of the hlist_node within the struct.
// */
//#define hlist_for_each_entry(pos, head, member)				\
//	for (pos = hlist_entry_safe((head)->first, typeof(*(pos)), member);\
//	     pos;							\
//	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

///**
// * hlist_for_each_entry_continue - iterate over a hlist continuing after current point
// * @pos:	the type * to use as a loop cursor.
// * @member:	the name of the hlist_node within the struct.
// */
//#define hlist_for_each_entry_continue(pos, member)			\
//	for (pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member);\
//	     pos;							\
//	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

///**
// * hlist_for_each_entry_from - iterate over a hlist continuing from current point
// * @pos:	the type * to use as a loop cursor.
// * @member:	the name of the hlist_node within the struct.
// */
//#define hlist_for_each_entry_from(pos, member)				\
//	for (; pos;							\
//	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

///**
// * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
// * @pos:	the type * to use as a loop cursor.
// * @n:		a &struct hlist_node to use as temporary storage
// * @head:	the head for your list.
// * @member:	the name of the hlist_node within the struct.
// */
//#define hlist_for_each_entry_safe(pos, n, head, member) 		\
//	for (pos = hlist_entry_safe((head)->first, typeof(*pos), member);\
//	     pos && ({ n = pos->member.next; 1; });			\
//	     pos = hlist_entry_safe(n, typeof(*pos), member))

///**
// * hlist_count_nodes - count nodes in the hlist
// * @head:	the head for your hlist.
// */
//static inline size_t hlist_count_nodes(struct hlist_head *head)
//{
//	struct hlist_node *pos;
//	size_t count = 0;

//	hlist_for_each(pos, head)
//		count++;

//	return count;
//}


static inline void slist_init(slist_t *node)
{
    node->next = NULL;
}

static inline void slist_append(slist_t *l, slist_t *n)
{
    struct slist_node *node;

    node = l;
    while (node->next) node = node->next;

    /* append the node to the tail */
    node->next = n;
    n->next = NULL;
}

static inline void slist_insert(slist_t *l, slist_t *n)
{
    n->next = l->next;
    l->next = n;
}

static inline unsigned int slist_len(const slist_t *l)
{
    unsigned int len = 0;
    const slist_t *list = l->next;
    while (list != NULL)
    {
        list = list->next;
        len ++;
    }

    return len;
}

static inline slist_t *slist_remove(slist_t *l, slist_t *n)
{
    /* remove slist head */
    struct slist_node *node = l;
    while (node->next && node->next != n) node = node->next;

    /* remove node */
    if (node->next != (slist_t *)0) node->next = node->next->next;

    return l;
}

static inline slist_t *slist_first(slist_t *l)
{
    return l->next;
}

static inline slist_t *slist_tail(slist_t *l)
{
    while (l->next) l = l->next;

    return l;
}

static inline slist_t *slist_next(slist_t *n)
{
    return n->next;
}

static inline int slist_isempty(slist_t *l)
{
    return l->next == NULL;
}

/**
 * @brief get the struct for this single list node
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define slist_entry(node, type, member) \
    container_of(node, type, member)

/**
 * rt_slist_for_each - iterate over a single list
 * @param pos the slist_t * to use as a loop cursor.
 * @param head the head for your single list.
 */
#define slist_for_each(pos, head) \
    for (pos = (head)->next; pos != NULL; pos = pos->next)

/**
 * rt_slist_for_each_entry  -   iterate over single list of given type
 * @param pos the type * to use as a loop cursor.
 * @param head the head for your single list.
 * @param member the name of the list_struct within the struct.
 */
#define slist_for_each_entry(pos, head, member) \
    for (pos = ((head)->next == (NULL) ? (NULL) : slist_entry((head)->next, typeof(*pos), member)); \
         pos != (NULL) && &pos->member != (NULL); \
         pos = (pos->member.next == (NULL) ? (NULL) : slist_entry(pos->member.next, typeof(*pos), member)))

/**
 * rt_slist_first_entry - get the first element from a slist
 * @param ptr the slist head to take the element from.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define slist_first_entry(ptr, type, member) \
    slist_entry((ptr)->next, type, member)

/**
 * slist_tail_entry - get the tail element from a slist
 * @param ptr the slist head to take the element from.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define slist_tail_entry(ptr, type, member) \
    slist_entry(slist_tail(ptr), type, member)

#endif /* __MY_LIST_H */

