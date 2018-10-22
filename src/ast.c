/*
 * TODO Functions of Abstract Syntax Tree
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#define out(a) printf("a\n")
char *opname[]={
#undef opxx
#define opxx(a, b) b,
#include "op.h"
	"Undefined Op"
};
int wlpdepth = 0;
int breakdest[256];
int continuedest[256];
bool isbb = FALSE;
int fnum = 0;
int labnum = 0;
bool isreturn = FALSE;
int blev = 0;
void
addParatoSymtab(Table ptab,ASTNode paralist)
{
	if(!paralist)return;
	Lnode stmts = paralist->block->stmts->first;
	Symbol s,s1;
	while ( stmts!=NULL )
	{
		s = ((ASTNode)(stmts->item))->sym;
		s1 = setDeclSym(ptab,s->name,s->val,s->type,s->lev+1,s->offset);
		s1->isPara = TRUE;
		stmts = stmts->next;
	}
}

ASTNode
newNumber(int value)
{
	ASTNode new;
	NEW0(new);
	new->kind = KValue;
	new->val = value;
	return new;
}

ASTNode
newName(Table ptab[], char *name)
{
	ASTNode new;
	NEW0(new);
	new->kind = KName;
	new->sym = getSym(ptab, name);
	if(new->sym==NULL);
	return new;
}

ASTNode
newPrefixExp(int op, ASTNode exp)
{
	ASTNode new;
	NEW0(new);
	new->kind = KPrefixExp;
	Exp newexp;
	NEW0(newexp);
	newexp->op = op;
	newexp->kids[0] = exp;
	new->exp = newexp;
	return new;
}

ASTNode
newParenExp(ASTNode exp)
{
	ASTNode new;
	NEW0(new);
	new->kind = KParenExp;
	Exp newexp;
	NEW0(newexp);
	newexp->op = -1;//操作符为-1
	newexp->kids[0] = exp;
	new->exp = newexp;
	return new;
}

ASTNode
newInfixExp(int op, ASTNode left, ASTNode right)
{
	ASTNode new;
	NEW0(new);
	new->kind = KInfixExp;
	Exp newexp;
	NEW0(newexp);
	newexp->op = op;
	newexp->kids[0] = left;
	newexp->kids[1] = right;
	if(right->kind==KValue && op==OP_DIV && right->val==0)yywarning("division by zero",NULL);
	new->exp = newexp;
	return new;
}

ASTNode
newAssignment(int op, ASTNode left, ASTNode right)
{
	ASTNode new;
	NEW0(new);
	new->kind = KAssignExp;
	Exp newexp;
	NEW0(newexp);
	newexp->op = op;
	newexp->kids[0] = left;
	newexp->kids[1] = right;
	new->exp = newexp;
	return new;
}


ASTNode
newBlock()
{
	ASTNode new;
	NEW0(new);
	new->kind = KBlock;
	Block newb;
	NEW0(newb);
	new->block = newb;
	newb->stmts = newList();
	NEW0(newb->symtrnode);
	newb->symtrnode->symtab = symtablist[lev];
	int levi=lev-1;
	while(!blocks[levi])levi--;
	newb->symtrnode->parent = (blocks[levi])->block->symtrnode;
	blocks[lev] = new;
	return new;
}

ASTNode
newEmptyStmt()
{
	ASTNode new;
	NEW0(new);
	new->kind = KEmptyStmt;
	return new;
}

ASTNode
newProgram()
{
	ASTNode new;
	NEW0(new);
	new->kind = KProgram;
	Block newb;
	NEW0(newb);
	new->block = newb;
	newb->stmts = newList();
	NEW0(newb->symtrnode);
	newb->symtrnode->parent = NULL;
	newb->symtrnode->symtab = symtablist[0];
	blocks[0] = new;
	return new;
}

ASTNode
newConstDecl()
{
	ASTNode new;
	NEW0(new);
	new->kind = KConstDecl;
	Block newb;
	NEW0(newb);
	new->block = newb;
	newb->stmts = newList();
	return new;
}

ASTNode
newVarDecl()
{
	ASTNode new;
	NEW0(new);
	new->kind = KVarDecl;
	Block newb;
	NEW0(newb);
	new->block = newb;
	newb->stmts = newList();
	return new;

}

ASTNode
newVarX(Table ptab,char*name)
{
	ASTNode new;
	NEW0(new);
	new->kind = KVarX;
	new->sym = setDeclSym(ptab,name,0,VAR_ID,lev,offset);
	return new;
}

ASTNode
newVarA(Table ptab,char*name,int elemtnum)
{
	ASTNode new;
	NEW0(new);
	new->kind = KVarA;
	Symbol s = setDeclSym(ptab,name,elemtnum,VARN_ID,lev,offset);
	Array constinit;
	NEW0(constinit);
	constinit->elemtnum = elemtnum;
	constinit->s = s;
	constinit->extra = NULL;
	new->arraymem = constinit;
	return new;
}

ASTNode
newFuncDecl_Pre(Table ptab,char* ident,ASTNode paralist,short type)
{
	ASTNode new;
	NEW0(new);
	new->kind = KFuncDecl;
	Function newfunc;
	NEW0(newfunc);
	newfunc->paralist = paralist;
	newfunc->funcsym = setDeclSym(ptab,ident,0,type,lev,offset);

	if(paralist!=NULL)
		newfunc->funcsym->extra = paralist->block->stmts;
	else newfunc->funcsym->extra = NULL;
	new->func = newfunc;

	return new;
}

void
addBodytoFunc(ASTNode funcnode,ASTNode funcbody)
{
	funcnode->func->funcbody = funcbody;
}
ASTNode
newParalist()
{
	ASTNode new;
	NEW0(new);
	new->kind = KParaList;
	Block newb;
	NEW0(newb);
	newb->stmts = newList();
	new->block = newb;
	return new;
}

ASTNode
newPara(char*name,short type)
{
	ASTNode new;
	NEW0(new);
	new->kind = KPara;
	Symbol news;
	NEW0(news);
	news->name = name;
	news->type = type;
	news->isInitial = FALSE;
	news->offset = offset;
	news->lev = 1;
	news->isPara = TRUE;
	new->sym = news;
	return new;
}

ASTNode
newCallStmt(ASTNode callnode)
{
	ASTNode new;
	NEW0(new);
	new->kind = KCallStmt;
	new->athnode = callnode;
	return new;
}

ASTNode
newCall(Table ptab,char* ident,ASTNode Realist)
{
	ASTNode new;
	NEW0(new);
	new->kind = KCall;
	Function newf;
	NEW0(newf);
	new->func = newf;
	new->func->funcsym = getFuncSym(ptab, ident);
	new->func->paralist = Realist;
	new->func->funcbody = NULL;

	if(new->func->funcsym==NULL)return new;
	List l = new->func->funcsym->extra;
	List lr = NULL;
	if(NULL==Realist)
	{
		lr = newList();
	}
	else{
		lr = Realist->block->stmts;
	}
	if(NULL == l) l = newList();
	if(l->size == lr->size)
	{
		//printf("l->size = %d\n",l->size);
		Lnode ln = l->first;
		Lnode lnr = lr->first;
		short type = 0;
		ASTNode  para;
		int size = l->size;

		while(ln!=NULL)
		{
			type = ((ASTNode)(ln->item))->sym->type;
			para = (ASTNode)(lnr->item);
			if(NULL!=para->sym)
				switch(type)
				{
					case VAR_ID:
					case CONST_ID:
						//printf("type = %d\n",type);
						if(para->kind == KName)
						{
							if(para->sym->type==VAR_ID||para->sym->type==CONST_ID)break;
							else yyerror("the argument is incomptible",para->sym->name);
						}
						else if( para->kind==KCall && para->func->funcsym->val==0)
							yyerror("the argument is incomptible",para->sym->name);
						break;
					case VARN_ID:
					case CONSTN_ID:
						if(para->kind == KName)
						{
							if(para->sym->type == VARN_ID ||para->sym->type == CONSTN_ID)break;

						}
						yyerror("the argument is incomptible",para->sym->name);
						break;
					default:;
				}
			ln = ln->next;
			lnr = lnr->next;
		}
	}
	else if(l->size<lr->size)yyerror("too more argument");
	else yyerror("too few argument");
	return new;
}

ASTNode
newIfStmt(ASTNode cond,ASTNode body)
{
	ASTNode new;
	NEW0(new);
	new->kind = KIf;
	Cond newcond;
	NEW0(newcond);
	new->cond = newcond;
	newcond->condition = cond;
	newcond->body[0] = body;
	return new;
}

ASTNode
newIfElStmt(ASTNode cond,ASTNode body1,ASTNode body2)
{
	ASTNode new;
	NEW0(new);
	new->kind = KIfEl;
	Cond newcond;
	NEW0(newcond);
	new->cond = newcond;
	newcond->condition = cond;
	newcond->body[0] = body1;
	newcond->body[1] = body2;
	return new;
}

ASTNode
newWhileStmt(ASTNode cond,ASTNode body)
{
	ASTNode new;
	NEW0(new);
	new->kind = KWhile;
	Cond newcond;
	NEW0(newcond);
	new->cond = newcond;
	newcond->condition = cond;
	newcond->body[0] = body;
	return new;
}

ASTNode
newConst(Table ptab,char *name,int val)
{
	ASTNode new;
	NEW0(new);
	new->kind = KConstN;
	//printf("new->kind = %d\n",new->kind);
	Symbol s;
	s = setDeclSym(ptab,name,val,CONST_ID,lev,offset);
	new->sym = s;
	return new;
}

ASTNode
newConstArray(Table ptab,char *name,ASTNode initials)
{
	ASTNode new;
	NEW0(new);
	new->kind = KConstArray;
	Array constinit;
	NEW0(constinit);
	constinit->elemtnum = 0;

	constinit->extra = initials;
	new->arraymem = constinit;
	Symbol s = setDeclSym(ptab,name,0,CONSTN_ID,lev,offset);
	constinit->s = s;
	ArrayInitial(s,initials);
	return new;
}

ASTNode
newConstArrayN(Table ptab,char *name,int elemtnum,ASTNode initials)
{
	ASTNode new;
	NEW0(new);
	new->kind = KConstArrayN;
	Array constinit;
	NEW0(constinit);

	constinit->extra = initials;
	constinit->elemtnum = elemtnum;
	if(elemtnum > (initials->block->stmts->size))
	{
		yywarning("too few elements of the initialization list",name);
	}
	else if(elemtnum < (initials->block->stmts->size))
	{
		yywarning("too more elements of the initialization list",name);
	}
	Symbol s = setDeclSym(ptab,name,elemtnum,CONSTN_ID,lev,offset);
	constinit->s = s;
	new->arraymem = constinit;
	ArrayInitial(s,initials);
	return new;
}

ASTNode
newInitials()
{
	ASTNode new;
	NEW0(new);
	new->kind = KInitials;
	Block newb;
	NEW0(newb);
	new->block = newb;
	newb->stmts = newList();
	return new;
}

ASTNode
newArrayQt(Table ptab[],char*name,ASTNode exp)
{
	ASTNode new;
	NEW0(new);
	new->kind = KArrayQt;
	Symbol s = getSym(ptab,name);
	Array constinit;
	NEW0(constinit);
	constinit->s = s;
	constinit->extra = exp;
	new->arraymem = constinit;
	return new;
}

ASTNode
newBreakStmt()
{
	ASTNode new;
	NEW0(new);
	new->kind = KBreakStmt;
	return new;
}
ASTNode
newContinueStmt()
{
	ASTNode new;
	NEW0(new);
	new->kind = KContinueStmt;
	return new;
}

ASTNode
newReturnStmt(ASTNode exp)
{
	ASTNode new;
	NEW0(new);
	new->kind = KReturnStmt;
	new->athnode = exp;
	return new;
}
ASTNode
newRealPARALIST()
{
	ASTNode new;
	NEW0(new);
	new->kind = KRealParaList;
	Block newb;
	NEW0(newb);
	newb->stmts = newList();
	new->block = newb;
	return new;
}

void
destroyExp(Exp *pnode)
{
	if (*pnode == NULL) return;
	Exp node = *pnode;
	destroyAST(&node->kids[0]);
	destroyAST(&node->kids[1]);
	free(node);
	*pnode = NULL;
}


void
destroyBlock(Block *pnode)
{
	if (*pnode == NULL) return;
	Block node = *pnode;
	destroyList(&node->stmts, destroyAST);
	if(NULL!=node->symtrnode)
	{
		destroyTable(&(node->symtrnode->symtab));
		free(node->symtrnode);
	}

	free(node);

	*pnode = NULL;
}

void
destroyArray(Array *parray)
{
	if(*parray==NULL) return;
	Array a = *parray;
	a->s = NULL;
	destroyAST(&(a->extra));
	free(a);
	*parray = NULL;
}

void
destroyInitials(Block *pnode)
{
	if(*pnode==NULL)return;
	if((*pnode)->stmts==NULL)
	{
		free(*pnode);
		*pnode = NULL;
		return;
	}
	Lnode l = (*pnode)->stmts->first;
	Lnode ln = NULL;
	int i=0;
	while(l!=NULL){
		ln = l->next;
		free((int*)l->item);
		free(l);
		l = ln;
	}
	free((*pnode)->stmts);
	free(*pnode);
	*pnode = NULL;
}

void destroyCond(Cond *pnode)
{
	if(*pnode == NULL)return;
	destroyAST(&((*pnode)->condition));
	destroyAST(&((*pnode)->body[0]));
	destroyAST(&((*pnode)->body[1]));
	free(*pnode);
	*pnode = NULL;
}

void destroyFunc(Function *pnode)
{
	if(*pnode == NULL) return;
	destroyAST(&((*pnode)->paralist));
	destroyAST(&((*pnode)->funcbody));
	(*pnode)->funcsym = NULL;
	free(*pnode);

	*pnode = NULL;
}

ASTTree
newAST()
{
	ASTTree new;
	NEW0(new);
	return new;
}

void
destroyAST(ASTNode *pnode)
{
	if (*pnode == NULL) return;
	ASTNode node = *pnode;
	int kind = node->kind;
	switch (kind) {
	case KVarX:
	case KConstN:
	case KValue:
	case KName:
	case KEmptyStmt:
	case KBreakStmt:
	case KContinueStmt:
	case KPara:
		break;
	case KPrefixExp:
	case KParenExp:
	case KInfixExp:
	case KAssignExp:
		destroyExp(&node->exp);
		break;
	case KBlock:
	case KProgram:
	case KConstDecl:
	case KVarDecl:
	case KParaList:
	case KRealParaList:
		destroyBlock(&node->block);
		break;
	case KVarA:
	case KConstArray:
	case KConstArrayN:
	case KArrayQt:
		destroyArray(&node->arraymem);
		break;
	case KInitials:
		destroyInitials(&node->block);
		break;
	case KIf:
	case KIfEl:
	case KWhile:
		destroyCond(&node->cond);
		break;
	case KFuncDecl:
		destroyFunc(&node->func);
		break;
	case KReturnStmt:
		destroyAST(&node->athnode);
		break;
	case KCall:
		free(node->func);
		node->func = NULL;
		break;
	case KCallStmt:
		destroyAST(&node->athnode);
		break;
	default:
		printf("Unhandled ASTNode kind!\n");
	}
	free(node);
	*pnode = NULL;
}

Loc
setLoc(ASTNode node, Loc loc)//fline, int fcol, int lline, int lcol)
{
	if (node->loc == NULL )
		NEW0(node->loc);
	node->loc->first_line = loc->first_line;
	node->loc->first_column = loc->first_column;
	node->loc->last_line = loc->last_line;
	node->loc->last_column = loc->last_column;
	return node->loc;
}

void
dumpAST(ASTNode node)
{
	if (node == NULL) return;
	int kind = node->kind;
	if(kind!=KBlock)isbb = FALSE;
	switch (kind) {
	case KVarDecl:
	{
		Lnode stmts = node->block->stmts->first;
		if (stmts!=NULL)
		{
			printf("int ");
			dumpAST((ASTNode)stmts->item);
			stmts = stmts->next;
		}
		while ( stmts!=NULL )
		{
			printf(", ");
			dumpAST((ASTNode)stmts->item);
			stmts = stmts->next;
		}
		printf(";\n");
		break;
	}

	case KVarX:
		printf("%s",node->sym->name);
		break;

	case KVarA:
		printf("%s[%d]",node->arraymem->s->name,node->arraymem->elemtnum);
		break;


	case KConstDecl:
	{
		Lnode stmts = node->block->stmts->first;
		if(stmts!=NULL)
		{
			printf("const int ");
			dumpAST((ASTNode)stmts->item);
			stmts = stmts->next;
		}
		while ( stmts!=NULL )
		{
			printf(", ");
			dumpAST((ASTNode)stmts->item);
			stmts = stmts->next;
		}
		printf(";\n");
		break;
	}

	case KConstN:
		printf("%s = %d",node->sym->name,node->sym->val);
		break;
	case KConstArray:
		printf("%s[] = ",node->arraymem->s->name);
		dumpAST(node->arraymem->extra);
		break;
	case KConstArrayN:
		KConstArray:
		printf("%s[%d] = ",node->arraymem->s->name,node->arraymem->elemtnum);
		dumpAST(node->arraymem->extra);
		break;
	case KInitials:
		{
			printf("{");
			Lnode stmts = node->block->stmts->first;
			if(stmts!=NULL)
			{
				printf("%d",*((int*)(stmts->item)));
				stmts = stmts->next;
			}
			while ( stmts!=NULL )
			{
				printf(", ");
				printf("%d",*((int*)(stmts->item)));
				stmts = stmts->next;
			}
			printf("}");
			break;
		}
	case KArrayQt:
		printf("%s[",node->arraymem->s->name);
		dumpAST(node->arraymem->extra);
		printf("]");
		break;
	case KValue:
		printf("%d", node->val);
		break;
	case KName:
		printf("%s", node->sym->name);
		break;
	case KPrefixExp:
		printf("%s", opname[node->exp->op]);
		dumpAST(node->exp->kids[0]);
		break;
	case KParenExp:
		printf("(");
		dumpAST(node->exp->kids[0]);
		printf(")");
		break;
	case KInfixExp:
		dumpAST(node->exp->kids[0]);
		printf("%s", opname[node->exp->op]);
		dumpAST(node->exp->kids[1]);
		break;
	case KAssignExp:
		dumpAST(node->exp->kids[0]);
		printf("%s", opname[node->exp->op]);
		dumpAST(node->exp->kids[1]);
		printf(";\n");
		break;
	case KIf:
		printf("if (");
		dumpAST(node->cond->condition);
		printf(")\n");
		//INDENTN(lev+1);
		dumpAST(node->cond->body[0]);
		break;
	case KIfEl:
		printf("if (");
		dumpAST(node->cond->condition);
		printf(")\n");
		dumpAST(node->cond->body[0]);
		INDENTN(lev);
		printf("else ");
		dumpAST(node->cond->body[1]);
		break;
	case KWhile:
		printf("while (");
		dumpAST(node->cond->condition);
		printf(")\n");
		dumpAST(node->cond->body[0]);
		break;
	case KBreakStmt:
		printf("break;\n");
		break;
	case KContinueStmt:
		printf("continue;\n");
		break;
	case KCallStmt:
		dumpAST(node->athnode);
		printf(";\n");
		break;
	case KCall:
		//printf("%d",node->kind==KCall);fflush(stdout);
		printf("%s(",node->func->funcsym->name);
		dumpAST(node->func->paralist);
		printf(")");
		break;
	case KReturnStmt:
		printf("return ");
		dumpAST(node->athnode);
		printf(";\n");
		break;
	case KFuncDecl:
	{
		if(node->func->funcsym->val)
			printf("int %s(",node->func->funcsym->name);
		else
			printf("void %s(",node->func->funcsym->name);
		dumpAST(node->func->paralist);
		printf(")\n");
		dumpAST(node->func->funcbody);
		break;
	}
	case KParaList:
	{
			Lnode stmts = node->block->stmts->first;
			if(stmts!=NULL)
			{
				dumpAST((ASTNode)(stmts->item));
				stmts = stmts->next;
			}
			while ( stmts!=NULL )
			{
				printf(", ");
				dumpAST((ASTNode)(stmts->item));
				stmts = stmts->next;
			}
			break;
	}
	case KRealParaList:
	{
		Lnode stmts = node->block->stmts->first;
		if(stmts!=NULL)
		{
			dumpAST((ASTNode)(stmts->item));
			stmts = stmts->next;
		}
		while ( stmts!=NULL )
		{
			printf(", ");
			dumpAST((ASTNode)(stmts->item));
			stmts = stmts->next;
		}
		break;
	}
	case KPara:
	{
		char* name = node->sym->name;
		switch (node->sym->type){
			case VAR_ID: 	printf("int %s",name);break;
			case VARN_ID:	printf("int %s[]",name);break;
			case CONST_ID:	printf("const int %s",name);break;
			case CONSTN_ID:	printf("const int %s[]",name);break;
			default:
				printf("paralist has illegal parament!\n");
		}
		break;
	}
	case KBlock:
	{
		if(isbb==FALSE)INDENTN(lev);
		lev++;
		printf("{\n");
#ifdef SYMTAB
		printf("/*this is the local symbols table\n");
		dumpTable(node->block->symtrnode->symtab);
		printf("table ends*/\n");
#endif
		Lnode stmts = node->block->stmts->first;
		while ( stmts!=NULL )
		{
			INDENTN(lev);
			isbb = TRUE;
			dumpAST((ASTNode)(stmts->item));
			stmts = stmts->next;
		}
		lev--;
		INDENTN(lev);
		printf("}\n");
		break;

	}
	case KProgram:
	{
#ifdef SYMTAB
		printf("/*this is the Global symbols table\n");
		dumpTable(node->block->symtrnode->symtab);
		printf("table ends*/\n");
#endif
		Lnode stmts = node->block->stmts->first;
		while ( stmts!=NULL )
		{
			dumpAST((ASTNode)(stmts->item));
			stmts = stmts->next;
			//printf(";\n");
		}
		break;
	}
	case KEmptyStmt:
		printf(";\n");
		break;
	default:
		printf("%dUnhandled ASTNode kind!\n",node->kind);
	}
}


void
gencode(ASTNode node)
{
	if (node == NULL) return;
	int kind = node->kind;
	switch (kind) {
	case KVarDecl:
	break;
	case KConstDecl:
	if(blev){
		Lnode stmts = node->block->stmts->first;
		while ( stmts!=NULL )
		{
			gencode((ASTNode)stmts->item);
			stmts = stmts->next;
		}
	}
	break;
	case KConstN:
		printf("movl $%d,%d(%%ebp)\n",node->sym->val,-4*node->sym->offset);
		break;
	case KConstArray:
	case KConstArrayN:
		{
			int i;
			int*xp = node->arraymem->s->extrap;
			int j = node->arraymem->elemtnum;
			for(i=0;i<j;i++)
				printf("movl $%d,%d(%%ebp)\n",*(xp+i),-4*(node->arraymem->s->offset+j-1-i));
		}
		break;
	case KName:
	{
		Symbol s = node->sym;
		if(s->lev==0)
		{
			if(s->type == CONST_ID || s->type == VAR_ID)
				printf("pushl %s\n",s->name);
			else
			{
				printf("leal %s,%%eax\n",s->name);
				printf("pushl %%eax\n");
			}
		}
		else
		{
			if(s->type == CONST_ID || s->type == VAR_ID)
				printf("pushl %d(%%ebp)\n",-4*s->offset);
			else
			{
				printf("leal %d(%%ebp),%%eax\n",-4*(s->offset+s->val-1));
				printf("pushl %%eax\n");
			}
		}
		break;
	}
	case KValue:
		printf("pushl $%d\n",node->val);
		break;
	case KArrayQt:
	{
		Symbol s = node->arraymem->s;
		gencode(node->arraymem->extra);
		printf("popl %%eax\n");
		if(s->lev==0)
			printf("pushl %s(,%%eax,4)\n",s->name);
		else if(s->isPara)
		{
			printf("movl %d(%%ebp),%%ebx\n",-4*s->offset);
			printf("leal 0(%%ebx,%%eax,4),%%ebx\n");
			printf("movl (%%ebx),%%eax\n");
			printf("pushl %%eax\n");
		}
		else
		{
			printf("pushl %d(%%ebp,%%eax,4)\n",-4*(s->offset+s->val-1));
		}
		break;
	}
	case KPrefixExp:
		gencode(node->exp->kids[0]);
		if(node->exp->op==OP_MINUS)
		{
			printf("popl %%eax\n");
			printf("negl %%eax\n");
			printf("pushl %%eax\n");
		}
		else if(node->exp->op==OP_ODD)
		{
			printf("popl %%eax\n");
			printf("testl $1,%%eax\n");
			printf("jz lab%d\n",labnum);
		}
		break;
	case KParenExp:
		gencode(node->exp->kids[0]);
		break;
	case KInfixExp:
		gencode(node->exp->kids[0]);
		//if(node->exp->kids[0]->kind==KCall)printf("pushl %%eax\n");
		gencode(node->exp->kids[1]);
		//if(node->exp->kids[1]->kind==KCall)printf("pushl %%eax\n");
		printf("popl %%ebx\n");
		printf("popl %%eax\n");
		switch(node->exp->op){
		case OP_MINUS:
		printf("subl %%ebx,%%eax\n");
		printf("pushl %%eax\n");
		break;
		case OP_PLUS:
		printf("addl %%ebx,%%eax\n");
		printf("pushl %%eax\n");
		break;
		case OP_MULT:
		printf("xor %%edx,%%edx\n");
		printf("imull %%ebx,%%eax\n");
		printf("pushl %%eax\n");
		break;
		case OP_DIV:
		printf("xor %%edx,%%edx\n");
		printf("idivl %%ebx\n");
		printf("pushl %%eax\n");
		break;
		case OP_MOD:
		printf("xor %%edx,%%edx\n");
		printf("idivl %%ebx\n");
		printf("pushl %%edx\n");
		break;
		case OP_EQL:
		printf("cmpl %%eax,%%ebx\n");
		printf("jne	lab%d\n",labnum);
		break;
		case OP_LEQ:
		printf("cmpl %%ebx,%%eax\n");
		printf("jg	lab%d\n",labnum);
		break;
		case OP_NEQ:
		printf("cmpl %%ebx,%%eax\n");
		printf("je	lab%d\n",labnum);
		break;
		case OP_LSS:
		printf("cmpl %%ebx,%%eax\n");
		printf("jge	lab%d\n",labnum);
		break;
		case OP_GEQ:
		printf("cmpl %%ebx,%%eax\n");
		printf("jl	lab%d\n",labnum);
		break;
		case OP_GTR:
		printf("cmpl %%ebx,%%eax\n");
		printf("jle	lab%d\n",labnum);
		break;
		default:;
		}
		break;
	case KAssignExp:
	{
		Symbol s;
		ASTNode nd = node->exp->kids[0];
		gencode(node->exp->kids[1]);
		if(nd->kind==KArrayQt)
		{
			s = node->exp->kids[0]->arraymem->s;
			gencode(node->exp->kids[0]->arraymem->extra);
			printf("popl %%ebx\n");
		}
		else s = node->exp->kids[0]->sym;
		printf("popl %%eax\n");
		if(nd->kind == KArrayQt)
		{
			if(s->lev==0)
			printf("movl %%eax,%s(,%%ebx,4)\n",s->name);
			else if(s->isPara)
			{
			printf("movl %d(%%ebp),%%ecx\n",-4*s->offset);
			printf("leal 0(%%ecx,%%ebx,4),%%ecx)\n");
			printf("movl %%eax,(%%ecx)\n");
			}
			else
			{
			printf("leal %d(%%ebp,%%ebx,4),%%ebx\n",-4*(s->offset+s->val-1));
			printf("movl %%eax,(%%ebx)\n");
			}
		}

		else if(s->lev==0)
			printf("movl %%eax,%s\n",s->name);
		else
			printf("movl %%eax,%d(%%ebp)\n",-4*s->offset);
		break;
	}
	case KBreakStmt:
		printf("jmp lab%d\n",breakdest[wlpdepth-1]);
		break;
	case KContinueStmt:
		printf("jmp lab%d\n",continuedest[wlpdepth-1]);
		break;
	case KIf:
	{
		int llab;
		gencode(node->cond->condition);
		llab = labnum;
		labnum++;
		gencode(node->cond->body[0]);
		printf("lab%d:\n",llab);
		break;
	}
	case KIfEl:
	{
		int llab1,llab2;
		gencode(node->cond->condition);
		llab1 = labnum;
		labnum++;
		gencode(node->cond->body[0]);
		printf("jmp lab%d\n",labnum);
		llab2 = labnum;
		labnum++;
		printf("lab%d:\n",llab1);
		gencode(node->cond->body[1]);
		printf("lab%d:\n",llab2);
		break;
	}
	case KReturnStmt:
	{
		if(NULL!=node->athnode)
		{
			gencode(node->athnode);
			printf("popl %%eax\n");
		}
		printf("jmp .LFRT%d\n",fnum);
		isreturn = TRUE;
		break;
	}

	case KWhile:
	{
		int llab1,llab2;
		printf("lab%d:\n",labnum);
		llab1 = labnum;
		labnum++;
		gencode(node->cond->condition);
		//printf("jz lab%d\n",labnum+1);
		llab2 = labnum;
		labnum++;
		breakdest[wlpdepth] = llab2;
		continuedest[wlpdepth] = llab1;
		wlpdepth++;
		gencode(node->cond->body[0]);
		wlpdepth--;
		printf("jmp lab%d\n",llab1);
		printf("lab%d:\n",llab2);
		break;
	}
	case KCallStmt:
		gencode(node->athnode);
		break;
	case KCall:
	{
		List l = node->func->paralist->block->stmts;
		if(l->size!=0)
		{
			Lnode lt = l->last;
			int i = 0;
			for(i=0;i<l->size;i++)
			{
				gencode((ASTNode)(lt->item));
				lt = lt->prev;
			}
		}
		printf("call %s\n",node->func->funcsym->name);
		printf("addl $%d,%%esp\n",l->size*4);
		printf("pushl %%eax\n");
		break;
	}
	case KFuncDecl:
	{
		Symbol s = node->func->funcsym;
		printf("\n.globl %s\n",s->name);
		printf(".type %s,@function\n",s->name);
		printf("%s:\n",s->name);
		printf(".LFB%d:\n",fnum);
		printf("pushl %%ebp\n");
		printf("movl %%esp,%%ebp\n");
		if(node->func->funcbody->block->offset!=1)
		printf("subl $%d,%%esp\n",4*node->func->funcbody->block->offset-4);
		gencode(node->func->funcbody);
		if(isreturn)
		{
			printf(".LFRT%d:\n",fnum);
			isreturn = FALSE;
		}
		printf("leave\n");
		printf("ret\n");
		printf(".LFE%d:\n",fnum);
		fnum++;
		printf(".size %s,.-%s\n",s->name,s->name);
		break;
	}
	case KBlock:
	{
		blev++;
		Lnode stmts = node->block->stmts->first;
		while ( stmts!=NULL )
		{
			gencode((ASTNode)(stmts->item));
			stmts = stmts->next;
		}
		blev--;
		break;
	}
	case KProgram:
	{
		Entry* bk;
		int i,j;
		Entry en;
		printf(".file \"a.c1\"\n");

		// build const block first
		printf("\n.section .rodata\n");
		bk = node->block->symtrnode->symtab->buckets;
		for(i=0;i<256;i++)
		{
			if(en = bk[i])
			{
				while(en)
				{
					if(en->sym.type == CONST_ID)
					{
						printf("\n.globl %s\n",en->sym.name);
						printf(".align 4\n");
						printf(".type %s, @object\n",en->sym.name);
						printf(".size %s, 4\n",en->sym.name);
						printf("%s:\n",en->sym.name);
						printf(".long %d\n",en->sym.val);
					}
					else if(en->sym.type == CONSTN_ID)
					{
						printf("\n.globl %s\n",en->sym.name);
						printf(".align 4\n");
						printf(".type %s, @object\n",en->sym.name);
						printf(".size %s, %d\n",en->sym.name,4*en->sym.val);
						printf("%s:\n",en->sym.name);
						for(j=0;j<en->sym.val;j++)
						{
							printf(".long %d\n",en->sym.extrap[j]);
						}
					}
					en = en->next;
				}
			}
		}
		printf("\n.data\n");
		for(i=0;i<256;i++)
		{
			if(en = bk[i])
			{
				while(en)
				{
					if(en->sym.type == VAR_ID)
					{
						printf("\n.globl %s\n",en->sym.name);
						printf(".align 4\n");
						printf(".type %s, @object\n",en->sym.name);
						printf(".size %s, 4\n",en->sym.name);
						printf("%s:\n",en->sym.name);
						printf(".long %d\n",en->sym.val);
					}else if(en->sym.type == VARN_ID)
					{
						printf(".comm %s,%d\n",en->sym.name,en->sym.val*4);
					}
					en = en->next;
				}
			}
		}
		printf("\n.text\n");
		printf(".globl _start\n");
		printf("_start:\n");
		printf("call main\nmovl %%eax,%%ebx\nmovl $1,%%eax\nint $0x80\n");
		Lnode stmts = node->block->stmts->first;
		while ( stmts!=NULL )
		{
			gencode((ASTNode)(stmts->item));
			stmts = stmts->next;
		}
		break;
	}
	default:
		printf("%dUnhandled ASTNode kind!\n",node->kind);
	}
}

