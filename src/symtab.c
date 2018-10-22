/*
 * Functions of Symbolic Table
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <common.h>

/**
 * Creates a symbolic table
 */
Table
newTable()
{
	Table new;
	NEW0(new);
	return new;
}

static void
destroyBucket(Entry *list)
{
	Entry node = *list, temp;

	while ( node != NULL ) {
		//printf("bucket destroy start\n");
		//printf("%s\n",node->sym.);
		temp = node->next;
		//printf("%s is being destroyed\n",node->sym.name);
		free(node->sym.extrap);
		free(node);
		node = temp;
		//printf("bucket destroy ended\n");
	}
	*list = NULL;
}


/**
 * Destroys the specified table
 */
void
destroyTable(Table *tab)
{
	int i=0;
	if (*tab == NULL)
		return;

	Entry *bucket = (*tab)->buckets, *bucket_end = (*tab)->buckets+256;
	while (bucket < bucket_end ) {
		destroyBucket(bucket);
		bucket++;
	}
	free(*tab);
	*tab = NULL;
}

// Look up the symbolic table to get the symbol with specified name
Symbol
lookup(Table ptab, const char *name)
{
	Entry pent;
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next)
	{
		if ( strcmp(name, pent->sym.name) == 0 )
		{
			return &pent->sym;
		}
	}
	return NULL;
}

// Get value of the specified name from the symbolic table
float
getVal(Table ptab, const char *name)
{
	Entry pent;
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next)
	{
		if ( strcmp(name, pent->sym.name) == 0 )
		{
			return pent->sym.val;
		}
	}

	NEW0(pent);
	pent->sym.name = (char *)name;
	pent->sym.val = 0;
	printf("Warn: %s(%d) was not initialized before, set it 0 as default\n", (char *)name, hashkey);
	pent->next = ptab->buckets[hashkey];
	ptab->buckets[hashkey] = pent;
	return 0;
}

Symbol
getSym(Table tablist[], const char *name)
{
	Entry pent;
	Table ptab;
	int i;
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for(i=lev;i>=0;i--)
	{
		ptab = tablist[i];
		for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next)
		{
			if ( strcmp(name, pent->sym.name) == 0 && pent->sym.type != FUNC_ID)
			{
				//if((type==FUNC_ID || pent->sym.type==FUNC_ID) && pent->sym.type!=type)continue;
				return &pent->sym;
			}
		}
	}
	yyerror("hasn't been declared!",name);
	return NULL;
}

Symbol
getFuncSym(Table ptab, const char *name)
{
	Entry pent;
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next)
	{
		if ( strcmp(name, pent->sym.name) == 0 && pent->sym.type == FUNC_ID)
		{
			return &pent->sym;
		}
	}
	yyerror("hasn't been declared",name);
	return NULL;
}

// Set value of the specified name into the symbolic table
//type = 0 var;1 const int;4 function;2 var [];3 const []
Symbol
setDeclSym(Table ptab, const char *name, int val,short type,int lv,int off)
{
	Entry pent;
	short type1=0;
	if(type == FUNC_VOID ||type == FUNC_INT){type1 = type;type=FUNC_ID;}
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next) {
		if ( strcmp(name, pent->sym.name) == 0) {
#if DEBUG
			if((type==FUNC_ID || pent->sym.type==FUNC_ID) && pent->sym.type!=type)continue;//只允许函数的标志符与其他类型的相同
			yyerror("is redeclared",name);
#endif
			return &pent->sym;
		}
	}
	NEW0(pent);
	pent->sym.name = (char *)name;
	pent->sym.type = type;//div
	pent->sym.lev = lv;
	pent->sym.offset = off;
	switch (type)
	{
	case VAR_ID:
		pent->sym.val = 0;//printf("0.%s=%d\n",name,val);
		pent->sym.isInitial = FALSE;
		pent->sym.extrap =NULL;
		break;
	case CONST_ID:
		pent->sym.val = val;//printf("1.%s=%d\n",name,val);
		pent->sym.isInitial = TRUE;
		pent->sym.extrap =NULL;
		//printf("%s = %d\n",pent->sym.name,pent->sym.val);
		break;
	case VARN_ID:
		pent->sym.val = val;//printf("2.%s=%d\n",name,val);
		pent->sym.isInitial = FALSE;
		pent->sym.extrap = (int*)malloc(sizeof(int)*val);
		break;
	case CONSTN_ID:
		pent->sym.val = val;
		pent->sym.isInitial = TRUE;
		pent->sym.val = val;
		pent->sym.extrap =NULL;//在另外的地方会初始化
		break;
	case FUNC_ID:
		pent->sym.val = (type1==FUNC_INT);//printf("4.%s=%d\n",name,val);
		pent->sym.isInitial = TRUE;
		pent->sym.extrap =NULL;
		break;
	default:
		printf("ERROR404\n");
	}
	pent->next = ptab->buckets[hashkey];
	ptab->buckets[hashkey] = pent;
	return &pent->sym;
}

Symbol
setVal(Table ptab, const char *name, int val)
{
	Entry pent;
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next) {
		if ( strcmp(name, pent->sym.name) == 0 ) {
			pent->sym.val = val;
#if DEBUG
			printf("update %s(%d)  %d\n", (char *)name, hashkey, val);
#endif
			return &pent->sym;
		}
	}
	NEW0(pent);
	pent->sym.name = (char *)name;
	pent->sym.val = val;
#if DEBUG
	printf("create %s(%d) %d\n", (char *)name, hashkey, val);
#endif
	pent->next = ptab->buckets[hashkey];
	ptab->buckets[hashkey] = pent;
	return &pent->sym;
}

void ArrayInitial(Symbol sym,ASTNode InitialNumbs)
{
	List l = InitialNumbs->block->stmts;
	Lnode ln = l->first;
	if(ln == NULL)
	{
		sym->val = 0;
		sym->extrap = NULL;
		printf("ERROR403\n");
		return;
	}
	int i = 0;
	sym->val = l->size;
	sym->extrap = (int*)malloc(sizeof(int)*(l->size));
	for(i=0;i<l->size;i++)
	{
		(sym->extrap)[i] = *((int*)(ln->item));
		ln = ln->next;
	}
}

void dumpSym(Symbol s)
{
	char* typename;
	switch(s->type)
	{
		case CONST_ID: typename = "const";break;
		case CONSTN_ID: typename = "const[]";break;
		case VAR_ID:	typename = "var";break;
		case VARN_ID:	typename = "var[]";break;
		case FUNC_ID:	typename = "func";break;
		default:
			typename = "error";
	}
	printf("%-10s%-8s%-8d%-8d",s->name,typename,s->val,s->isInitial);
	if(s->extrap)
	{
		int* np=s->extrap;
		int i = 0;
		printf("%d",np[i]);
		i++;
		for(;i<s->val;i++)printf(",%d",np[i]);
	}
}

void dumpTable(Table ptab)
{
	int i = 0;
	Entry entry = NULL;
	printf("name\ttype\tvalue\tisIntl\textrap\n");
	for(i=0;i<256;i++)
	{
		entry = ptab->buckets[i];
		while(entry!=NULL)
		{
			dumpSym(&entry->sym);
			printf("\n");
			entry =  entry->next;
		}
	}
}

