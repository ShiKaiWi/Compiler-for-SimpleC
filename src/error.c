/*
 * Functions of Error Messages and their Container
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

char *errmsgs[] = {
#undef errxx
#define errxx(a, b) b,
#include "errcfg.h"
	"Other Errors"
};

/**
 * Creates an error message and inserts it into the error list 
 * of the specified error factory
 */
Errmsg
newError(ErrFactory errfactory, int type, int line, int col)
{
	Errmsg new;
	NEW0(new);
	new->isWarn = FALSE;
	new->type = type;
	new->msg = errmsgs[type];
	new->line = line;	
	new->column = col;
	listaddItem(errfactory->errors, new);	
	return new;
}

/**
 * Creates a warning message and inserts it into the warning list 
 * of the specified error factory
 */
Errmsg
newWarning(ErrFactory errfactory, int type, int line, int col)
{
	Errmsg new;
	NEW0(new);
	new->isWarn = TRUE;
	new->type = type;
	new->msg = errmsgs[type];
	new->line = line;	
	new->column = col;
	listaddItem(errfactory->warnings, new);	
	return new;
}

/**
 * Destroy an error/warning message.
 */
void
destroyErrmsg(Errmsg *msg)
{
	if (*msg == NULL) return;
	free(*msg);
	*msg = NULL;
}
/**
 * Dumps an error message
 */
void
dumpErrmsg(Errmsg error)
{
	if ( error->isWarn )
		printf(" Warning@");
	else
		printf(" Error@");
	printf("(%d, %d): %s\n", error->line, error->column, error->msg);
}

ErrFactory
newErrFactory()
{
	ErrFactory new;
	NEW(new);
	new->errors = newList();
	new->warnings = newList();
	return new;
}

/**
 * Dumps all error messages
 */
void
dumpErrors(ErrFactory errfactory)
{
	List errors = errfactory->errors;
	int i = 0, size = errors->size;
	printf("\nFound %d errors at least:\n", 
		size);
	
	ListItr itr = getGListItr(errors, 0);
	
	Errmsg msg;
	while( hasNext(itr) ) {
		msg = (Errmsg)nextItem(itr);
		dumpErrmsg(msg);
	}
}

void
dumpWarnings(ErrFactory errfactory)
{
	List warns = errfactory->warnings;
	int i = 0, size = warns->size;
	printf("\nFound %d warnings at least:\n", 
		size);
	
	ListItr itr = getGListItr(warns, 0);
	
	Errmsg msg;
	while( hasNext(itr) ) {
		msg = (Errmsg)nextItem(itr);
		dumpErrmsg(msg);
	}
}

/**
 * Destroys the specified error factory
 */
void
destroyErrFactory(ErrFactory *errfact)
{
	destroyList(&(*errfact)->errors, &destroyErrmsg);
	destroyList(&(*errfact)->warnings, &destroyErrmsg);
	free(*errfact);
	*errfact = NULL;
}
