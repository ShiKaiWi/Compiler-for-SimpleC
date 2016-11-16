/* 
 * Common definitions and declarations for compilers 
 */
#ifndef _COMMON_H_
#define _COMMON_H_
#include "util.h"

// operator kinds
// You could add more kinds of error messages into op.h 
enum {
#define opxx(a, b) OP_##a,
#include "op.h"
	OPLAST
};
#define CONST_ID	134
#define VAR_ID  	135
#define CONSTN_ID	136
#define VARN_ID		137
#define FUNC_ID		138
#define FUNC_VOID   139
#define FUNC_INT 140
#define INDENTN(n) {int i;for(i=0;i<n;i++)printf("\t");}
//extern char **opname;

// symbolic table
typedef struct symbol {
	char	*name;	// name of the symbol
	bool	isInitial;	// whether it is initialized
	bool	isPara;	
	short	type;	// type of the symbol
	int		val;	// value of the symbol
	int*	extrap;
	List 	extra;
	int offset;
	int lev;
} *Symbol;

typedef struct entry {
	struct symbol sym;
	struct entry *next;
} *Entry;

typedef struct table {
	// a hashtable to store symbols
	struct entry *buckets[256];
} *Table;
#define HASHSIZE 256

// Function declarations corresponding to symbolic table
Table 	newTable();
Symbol 	getFuncSym(Table ptab,const char*name);
Symbol 	lookup(Table ptab, const char *name);
Symbol 	getSym(Table ptab[], const char *name);
float 	getVal(Table ptab, const char *name);
Symbol 	setVal(Table ptab, const char *name, int val);
Symbol 	setDeclSym(Table ptab, const char *name,int val,short type,int lv,int off);
void 	destroyTable();
void 	dumpTable(Table ptab);
void 	dumpSym(Symbol s);
// Error/warning message
// You could add more kinds of error messages into errcfg.h 
enum {
#define errxx(a, b) a,
#include "errcfg.h"
	LASTERR
};

// An error/warning message
typedef struct errmsg {
	bool	isWarn;
	int 	type;
	char 	*msg;
	int	line;
	int	column;
} *Errmsg;

// Error factory
typedef struct errfactory { 
	List	errors;
	List	warnings;
} *ErrFactory;

// Function declarations on error message management
Errmsg	newError	(ErrFactory errfactory, int type, int line, int col);
Errmsg	newWarning	(ErrFactory errfactory, int type, int line, int col);
void	dumpErrmsg	(Errmsg error);
ErrFactory newErrFactory();
void	dumpErrors	(ErrFactory errfactory);
void	dumpWarnings	(ErrFactory errfactory);
void	destroyErrFactory(ErrFactory *errfact);

// abstract syntax tree
// Structure for tracking locations, same as YYLTYPE in y.tab.h
typedef struct location {
	int first_line;
	int first_column;
	int last_line;
	int last_column;
} *Loc;

typedef struct {
	int 	op;
	//int type;
	//int 	val;
	struct astnode	*kids[2];// kids of the AST node
} *Exp;

typedef struct{
	struct astnode *condition;
	struct astnode *body[2];
}*Cond;

/*typedef struct {
	struct astnode *exp;
} *ExpStmt;*/

typedef struct symtrnode{
	Table symtab;
	struct symtrnode* parent;
}*Symtrnode;

typedef struct {
	List  stmts;
	Symtrnode symtrnode;
	int offset;
} *Block;

typedef struct{
	struct astnode *funcbody;
	struct astnode *paralist;
	Symbol funcsym;
}*Function;

typedef struct{
	int elemtnum;
	Symbol s;
	struct astnode *extra;
}*Array;

typedef struct astnode{
	enum {
		KValue = 200,		// numerial value:
		KName,			// name, such as variable name
		KPrefixExp,		// prefix expression
		KInfixExp,		// infix expression
		KAssignExp,		// assignment expression
		KParenExp,		// parentheses expression 205
		//KExpStmt,		// expression statement
		KBlock,			// block 206
		KProgram,
		KConstDecl,
		KVarDecl,//209
		KFuncDecl,
		KCallStmt,
		KIf,
		KIfEl,
		KWhile,
		KConstN,//215
		KConstArray,
		KInitials,
		KConstArrayN,
		KVarX,
		KVarA,//220
		KArrayQt,
		KEmptyStmt,
		KBreakStmt,
		KContinueStmt,
		KReturnStmt,//225
		KParaList,
		KPara,
		KRealParaList,
		KCall
	} kind;	// kind of the AST node
	union {		// information of various kinds of AST node 
		int  val;		// KValue: numerial value
		Symbol sym;		// KName: symbols var const function
		Exp   exp;		// KPrefixExp,
					// KInfixExp,
					// KAssignExp,
					// KParenExp
		//ExpStmt  estmt;		// KExpStmt
		Block  block;		// KBlock,function
		struct astnode* athnode;
		Function func;
		Cond cond;
		Array arraymem;
	};
	Loc 	loc;			// locations
} *ASTNode;

typedef struct ASTtree {
	ASTNode root;
} *ASTTree;

// functions for creating various kinds of ASTnodes
void 	addParatoSymtab(Table ptab,ASTNode paralist);
ASTNode newNumber(int value);
ASTNode newName(Table ptab[], char *name);
ASTNode newPrefixExp(int op, ASTNode exp);
ASTNode newParenExp(ASTNode exp);
ASTNode newInfixExp(int op, ASTNode left, ASTNode right);
ASTNode newAssignment(int op, ASTNode left, ASTNode right);
void	destroyExp(Exp *pexp);
ASTNode newExpStmt(ASTNode exp);
//void	destroyExpStmt(ExpStmt *pexpstmt);
ASTNode newBlock();
//diy
ASTNode newProgram();
ASTNode newConstDecl();
ASTNode newVarDecl();
ASTNode newFuncDecl_Pre(Table ptab,char* ident,ASTNode paralist,short type);
void 	addBodytoFunc(ASTNode funcnode,ASTNode funcbody);
ASTNode newConst(Table ptab,char *name,int val);
ASTNode	newCall(Table ptab,char* ident,ASTNode Realist);
ASTNode newIfStmt(ASTNode cond,ASTNode body);
ASTNode newIfElStmt(ASTNode cond,ASTNode body1,ASTNode body2);
ASTNode newWhileStmt(ASTNode cond,ASTNode body);
ASTNode newConstArray(Table ptab,char *name,ASTNode initials);
ASTNode newConstArrayN(Table ptab,char *name,int elemtnum,ASTNode initials);
ASTNode newInitials();
ASTNode newVarX(Table ptab,char*name);
ASTNode newVarA(Table ptab,char*name,int elemtnum);
ASTNode newArrayQt(Table ptab[],char*name,ASTNode exp);
ASTNode newEmptyStmt();
ASTNode newBreakStmt();
ASTNode newContinueStmt();
ASTNode newReturnStmt(ASTNode exp);
ASTNode newParalist();
ASTNode newPara(char*name,short type);
ASTNode	newRealPARALIST();
ASTNode newCallStmt(ASTNode callnode);
ASTTree newAST();

void	destroyBlock(Block *pblock);
void 	destroyExp(Exp *pnode);
void	destroyAST(ASTNode *pnode);
void  	destroyArray(Array *parray);
void  	destroyInitials(Block *pnode);
void 	destroyCond(Cond *pnode);
void 	destroyFunc(Function *pnode);

void 	dumpAST(ASTNode node);
void 	gencode(ASTNode node);

extern Table symtablist[];
extern ASTNode blocks[];
extern char lev;
extern int errnum;
extern int offset;
Loc	setLoc(ASTNode node, Loc loc);

#endif // !def(_COMMON_H_)
