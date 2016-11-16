/* 
 * Utility definitions and declarations 
 * -- list, list iterator
 */

#ifndef _UTIL_H_
#define _UTIL_H_
#define NEW(p) ((p) = malloc(sizeof *(p)))
#define NEW0(p) memset(NEW(p), 0, sizeof *(p))
#define bool	char
#define TRUE	1
#define FALSE	0
#ifdef DEBUG
#define debug(a) printf("%s\n", a)
#else
#define debug(a)
#endif 

typedef struct lnode{
	void	*item;	// pointer to the item.	
	void 	*next;	// pointer to the next node
	void 	*prev;	// pointer to the previous node
} *Lnode;

typedef struct {
	int 	size;	// number of nodes
	Lnode 	first;	// pointer to the first node
	Lnode 	last;	// pointer to the last node
} *List;

Lnode 	newLnode	(void *item);
List 	newList		(); 
void 	*getFirst	(List list);
void 	*getLast	(List list);
void 	*removeFirst	(List list);
void 	*removeLast	(List list);
void 	addFirst	(List list, void *item);
void 	addLast		(List list, void *item);
int  	indexof		(List list, void *item);
bool	listcontains	(List list, void *item);
int	listsize	(List list);
bool	listaddItem	(List list, void *item);
bool	listremoveItem	(List list, void *item);
void 	listclear	(List list, void (*destroyItem)());
Lnode	listnode	(List list, int index);
void	*listget	(List list, int index);
void	*listset	(List list, int index, void *item);
void	listadd	(List list, int index, void *item);
void	*listremove	(List list, int index);
void 	destroyList	(List *list, void (*destroyItem)());

// List Iterator
typedef struct {
	List	list;
	int	nextIndex;
	Lnode	lastRet;
	Lnode	next;
} *ListItr;

ListItr newListItr	(List list, int index);
ListItr getGListItr	(List list, int index);
void 	resetListItr	(ListItr itr, List list, int index);
bool	hasNext		(ListItr itr);
void	*nextItem	(ListItr itr);
bool	hasPrevious	(ListItr itr);
void	*prevItem	(ListItr itr);
void	destroyListItr	(ListItr *itr);
#endif //!def(_UTIL_H_)
