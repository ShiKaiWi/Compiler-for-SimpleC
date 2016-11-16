/*
 * Functions of List
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "util.h"

static ListItr glistitr = NULL;

// new an error or a warn message
Lnode
newLnode(void *item)
{
	Lnode new;
	NEW0(new);
	new->item = item;
	return new;
}

List
newList()
{
	List new;
	NEW0(new);
	return new;
}

/**
 *  Links element item as first element of the list
 */
static void
linkFirst(List list, void *item)
{
	Lnode first = list->first;
	Lnode newnode = newLnode(item);
	newnode->next = first;
	newnode->prev = NULL;
	if ( first == NULL )
		list->last = newnode;
	else
		first->prev = newnode;
	list->first = newnode;
	list->size ++;
}

/**
 *  Links element item as last element of the list
 */
static void
linkLast(List list, void *item)
{
	Lnode last = list->last;
	Lnode newnode = newLnode(item);
	newnode->prev = last;
	newnode->next = NULL;
	if ( last == NULL )
		list->first = newnode;
	else
		last->next = newnode;
	list->last = newnode;
	list->size ++;
}

/**
 *  Inserts element item before non-NULL node succ
 */
static void
linkBefore(List list, void *item, Lnode succ)
{
	assert(succ != NULL);
	Lnode pred = succ->prev;
	Lnode newnode = newLnode(item);
	newnode->next = succ;
	newnode->prev = succ->prev;
	succ->prev = newnode;
	if ( pred == NULL )
		list->first = newnode;
	else
		pred->next = newnode;
	list->size ++;
}

/**
 *  Unlinks non-NULL node 
 */
static void *
unlink(List list, Lnode node)
{
	assert(node != NULL);
	Lnode 	prev, next;
	void 	*item;

	item = node->item;
	prev = node->prev;
	next = node->next;
	if (prev == NULL)
		list->first = next;
	else {
		prev->next = next;
		node->prev = NULL;
	}
	if (next == NULL)
		list->last = prev;
	else {
		next->prev = prev;
		node->next = NULL;
	}

	node->item = NULL;
	free(node);
	list->size--;
	return item;
}

/**
 *  Returns the first element in this list
 */
void *
getFirst(List list)
{
	Lnode first = list->first;
	if ( first == NULL ) {
		printf("The list is empty!\n");
		exit(-1);
	}
	return first->item;
}

/**
 *  Returns the last element in this list
 */
void *
getLast(List list)
{
	Lnode last = list->last;
	if ( last == NULL ) {
		printf("The list is empty!\n");
		exit(-1);
	}
	return last->item;
}

/**
 *  Removes and returns the first element from this list
 */
void *
removeFirst(List list)
{
	Lnode first = list->first;
	if ( first == NULL ) {
		printf("The list is empty!\n");
		exit(-1);
	}
	return unlink(list, first);
}

/**
 *  Removes and returns the last element from this list
 */
void *
removeLast(List list)
{
	Lnode last = list->last;
	if ( last == NULL ) {
		printf("The list is empty!\n");
		exit(-1);
	}
	return unlink(list, last);
}

/**
 * Inserts the specified element at the beginning of the list
 */
void
addFirst(List list, void * item)
{
	linkFirst(list, item);
}

/**
 * Inserts the specified element to the end of the list
 */
void
addLast(List list, void * item)
{//将语句连接起来
	linkLast(list, item);
}

/**
 * Returns the index of the first occurence of the specified 
 * element in the list, or -1 if the list does not contain
 * the element.
 */
int
indexof(List list, void *item)
{
	int index = 0;
	Lnode node = list->first;
	while (node != NULL) {
		if ( node->item = item )
			return index;
		index ++;
		node = node->next;
	}
	return -1;
}

/**
 * Returns TRUE if the list contains the specified element.
 */
bool
listcontains(List list, void *item) 
{
	return indexof(list, item) != -1;
}

/**
 * Returns the number of elements in the list
 */
int
listsize(List list)
{
	return list->size;
}

/**
 * Appends the specified element to the end of the list
 */
bool
listaddItem(List list, void * item)
{
	linkLast(list, item);
	return TRUE;
}

/**
 * Removes the first occurrence of the specified element from
 * the list, if it is present.
 */
bool
listremoveItem(List list, void *item)
{
	Lnode node = list->first;
	while(node!=NULL) {
		if (node->item == item) {
			unlink(list, node);
			return TRUE;
		}
		node = node->next;
	}
	return FALSE;
}

/**
 * Removes all of the elements from the list
 */
void
listclear(List list, void destroyItem())
{
	Lnode node = list->first;
	while (node != NULL) {
		Lnode next = node->next;
		if (destroyItem != NULL)
			destroyItem(&node->item);
		node->item = NULL;
		free(node);
		node = next;
	}
	list->first = list->last = NULL;
	list->size = 0;
}

/**
 * Returns the node at the specified element index
 */
Lnode
listnode(List list, int index)
{
	Lnode node;
	int i;
	if ( index < (list->size >> 1)) {
		node = list->first;
		for ( i = 0; i < index; i++)
			node = node->next;
		return node;
	} else {
		node = list->last;
		for ( i = list->size - 1; i > index; i--)
			node = node->prev;
		return node;
	}
}

/**
 * Returns the element at the specified position in the list
 */
void *
listget(List list, int index)
{
	Lnode node = listnode(list, index);
	return node==NULL ? NULL : node->item;
}

/**
 * Replaces the element at the specified position in the list
 */
void *
listset(List list, int index, void *item)
{
	Lnode node = listnode(list, index);
	if ( node==NULL ) {
		printf("Index %d is out of the bound!\n", index);
		exit(-1);
	}
	void *oldval = node->item;
	node->item = item;	
	return oldval;
}

/**
 * Inserts the specified element at the specified position in the 
 * list
 */
void 
listadd(List list, int index, void *item)
{
	if ( index == list->size ) {
		linkLast(list, item);
		return;
	}
	Lnode node = listnode(list, index);
	if ( node==NULL ) {
		printf("Index %d is out of the bound!\n", index);
		exit(-1);
	}
	linkBefore(list, item, node);
}

/**
 * Removes the element at the specified position in the list
 */
void *
listremove(List list, int index)
{
	Lnode node = listnode(list, index);
	if ( node==NULL ) {
		printf("Index %d is out of the bound!\n", index);
		exit(-1);
	}
	return unlink(list, node);
}

/**
 * Destroys the list
 */
void
destroyList(List *list, void destroyItem())
{
	listclear(*list, destroyItem);
	free(*list);
	*list = NULL;
}
/*************************************************************
 * Functions for iterating elements
 ************************************************************/

/**
 * Creates an iterator
 */
ListItr
newListItr(List list, int index)
{
	ListItr new;
	NEW0(new);
	new->list = list;
	new->next = listnode(list, index);
	new->nextIndex = index;
	return new;
}

/**
 * Gets the singleton global List iterator
 */
ListItr
getGListItr(List list, int index)
{
	if ( glistitr == NULL )
	NEW0(glistitr);
	glistitr->list = list;
	glistitr->next = listnode(list, index);
	glistitr->nextIndex = index;
	return glistitr;
}

/**
 * Resets the list iterator
 */
void
resetListItr(ListItr itr, List list, int index)
{
	assert(itr != NULL);
	itr->list = list;
	itr->next = listnode(list, index);
	itr->nextIndex = index;
}

/**
 * Returns TRUE if there is next element
 */
bool
hasNext(ListItr itr)
{
	return itr->nextIndex < itr->list->size;
}

/**
 * Returns the next element
 */
void *
nextItem(ListItr itr)
{
	if ( !hasNext(itr) ) {
		printf("Already reach the end of the list.\n");
		return NULL;
	}
	itr->lastRet = itr->next;
	itr->next = itr->next->next;
	itr->nextIndex ++;
	return itr->lastRet->item;
}

/**
 * Returns TRUE if there is previous element
 */
bool
hasPrevious(ListItr itr)
{
	return itr->nextIndex > 0;
}

/**
 * Returns the previous element
 */
void *
prevItem(ListItr itr)
{
	if ( !hasPrevious(itr) ) {
		printf("Already reach the beginning of the list.\n");
		return NULL;
	}
	if ( itr->next == NULL )
		itr->next = itr->list->last;
	else
		itr->next = itr->next->prev;
	
	itr->lastRet = itr->next;
	itr->nextIndex --;
	return itr->lastRet->item;
}

/**
 * Destroys the iterator
 */
void
destroyListItr(ListItr *itr)
{
	free(*itr);
	*itr = NULL;
}
