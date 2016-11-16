
%{
#include <stdio.h>
#include <common.h>
#include <string.h>
Table Gsymtab;
ASTTree ast;
ASTNode tempnode;
ASTNode tempnode_func;
ASTNode tempnode_paralist;
ASTNode tempnode_block;
Symbol tempsym;
Table symtablist[256];
ASTNode blocks[256];
int offset=0;
int* tempint;
char lev=0;
int errnum=0;
int warnnum=0;
int lpdepth=0;
bool returnvalue = FALSE;
bool returnerr;
bool isfuncbody;
%}

%union {
	//float fval;
	char *name;
	int  ival;
	ASTNode node;
}
%expect 7
%locations
%token NUMBER ident CONST VOID INT COMMA SEMICOLON IF ELSE WHILE odd	BREAK CONTINUE RETURN
%token PLUS MINUS MULT DIV ASGN MOD neq lss gtr leq geq eql
%token LB RB LSB RSB LLB RLB

%left  MINUS PLUS
%left  MULT DIV MOD

%type  <ival> PLUS MINUS MULT DIV ASGN MOD neq lss gtr leq geq eql LB RB LSB RSB LLB RLB CONST VOID INT COMMA SEMICOLON IF ELSE WHILE odd RelOp NUMBER
%type  <name> ident
%type  <node>  FuncDef CompUnit Decl CONSTDecl CONSTDef VarDecl Var Stmt Block BlockItem EXP LVal Cond ArrayInitial PARA PARALIST CALL RealPARALIST Stmt_noSemi

%%
Target	:
			CompUnit
			{
				ast->root = $1;
				if(NULL == getFuncSym(Gsymtab,"main"))yyerror("No main() function",NULL);
			}
			;
	
CompUnit:	{$$ = newProgram();}
			|CompUnit Decl
				{	
					addLast($1->block->stmts, $2);
					$$ = $1;
					setLoc($$, (Loc)&(@$));
				}
			|CompUnit  FuncDef
				{
					addLast($1->block->stmts,$2);
					$$ = $1;
					setLoc($$, (Loc)&(@$));				
				}
			;
Decl:		CONSTDecl
				{
					$$ = $1;
					setLoc($$, (Loc)&(@$));	
				}
			|VarDecl
				{	//debug("vardecl");
					$$ = $1;	
					setLoc($$, (Loc)&(@$));					
				}
			;
CONSTDecl:	CONST INT CONSTDef SEMICOLON
				{
					$$ = $3;
					setLoc($$, (Loc)&(@$));	
				}
			|CONST INT CONSTDef
				{
					yyerror("';' expected at the end of the  statement",NULL);
					$$ = $3;
					setLoc($$, (Loc)&(@$));	
				}
			;

CONSTDef:	CONSTDef COMMA ident ASGN NUMBER
				{
					tempnode = newConst(symtablist[lev],$3,$5);
					addLast($1->block->stmts,tempnode);
					$$ = $1;
					offset++;
					setLoc($$, (Loc)&(@$));	
				}
			|CONSTDef COMMA ident LSB RSB ASGN LLB ArrayInitial RLB
				{
					tempnode = newConstArray(symtablist[lev],$3,$8);
					addLast($1->block->stmts,tempnode);
					offset+=$8->block->stmts->size;
					$$ = $1;
					setLoc($$, (Loc)&(@$));	
				}
			|CONSTDef COMMA ident LSB NUMBER RSB ASGN LLB ArrayInitial RLB
				{
					tempnode = newConstArrayN(symtablist[lev],$3,$5,$9);
					addLast($1->block->stmts,tempnode);
					offset+=$9->block->stmts->size;
					$$ = $1;
					setLoc($$, (Loc)&(@$));	
				}
			|ident ASGN NUMBER
				{	
					$$ = newConstDecl();
					tempnode = newConst(symtablist[lev],$1,$3);				
					addLast($$->block->stmts,tempnode);
					offset++;
					setLoc($$, (Loc)&(@$));	
				}
			|ident LSB RSB ASGN LLB ArrayInitial RLB
				{
					
					$$ = newConstDecl();
					tempnode = newConstArray(symtablist[lev],$1,$6);
					addLast($$->block->stmts,tempnode);
					offset+=$6->block->stmts->size;
					setLoc($$, (Loc)&(@$));	
				}
			|ident LSB NUMBER RSB ASGN LLB ArrayInitial RLB
				{//debug("const [n] define");
					$$ = newConstDecl();
					tempnode = newConstArrayN(symtablist[lev],$1,$3,$7);
					addLast($$->block->stmts,tempnode);
					offset+=$7->block->stmts->size;
					setLoc($$, (Loc)&(@$));	
				//debug("const [n] defined");
				}
			
			;

ArrayInitial:	
					{
						$$ = newInitials();					
					}
				|ArrayInitial COMMA NUMBER
					{
						tempint = (int*)malloc(sizeof(int));
						*tempint = $3; 
						addLast($1->block->stmts,tempint);	
						$$ = $1;		
						setLoc($$, (Loc)&(@$));			
					}
				|NUMBER
					{	
						//debug("arrayinitial ->number");
						$$ = newInitials();	
						tempint = (int*)malloc(sizeof(int));
						*tempint = $1; 
						addLast($$->block->stmts,tempint);
						setLoc($$, (Loc)&(@$));	
						//debug("arrayinitialist is done");
					}
				;

VarDecl:	INT Var SEMICOLON
				{
					//debug("var->int x");
					$$ = $2;
					setLoc($$, (Loc)&(@$));										
				}
			|INT Var 
				{
					yyerror("';' expected at the end of the  statement",NULL);
					$$ = $2;
					setLoc($$, (Loc)&(@$));	
				}
			;
Var:		Var COMMA ident
				{
					tempnode = newVarX(symtablist[lev],$3);
					offset++;
					addLast($1->block->stmts,tempnode);
					$$ = $1;
					setLoc($$, (Loc)&(@$));	
				}
			|Var COMMA ident LSB NUMBER RSB
				{//debug("var->  ident[]");
					tempnode = newVarA(symtablist[lev],$3,$5);
					addLast($1->block->stmts,tempnode);
					offset+=$5;
					$$ = $1;
					setLoc($$, (Loc)&(@$));						
				}	
			|ident LSB NUMBER RSB
				{	//debug("var->  ident[]");
					$$ = newVarDecl();
					tempnode = newVarA(symtablist[lev],$1,$3);
					addLast($$->block->stmts,tempnode);	
					offset+=$3;
					setLoc($$, (Loc)&(@$));					
				}
			|ident
				{
					$$ = newVarDecl();
					tempnode = newVarX(symtablist[lev],$1);			
					addLast($$->block->stmts,tempnode);	
					offset++;
					setLoc($$, (Loc)&(@$));				
				}			
			;

FuncDef:	VOID ident LB {offset = -2;}PARALIST RB 
			{
				//debug("funcdef ->void2.5");
				tempnode_paralist = $5;returnvalue=FALSE;returnerr = TRUE;
				tempnode_func = newFuncDecl_Pre(symtablist[lev],$2,$5,FUNC_VOID);
				isfuncbody = TRUE;
				offset = 1;
				//debug("funcdef ->void3");
			} 
			Block
				{
					//debug("funcdef ->void");
					$$ = tempnode_func;
					addBodytoFunc($$,$8);
					if(returnerr==FALSE)yywarning("'return' with a value, in function returning void",$2);
					setLoc($$, (Loc)&(@$));	
				}
			|INT ident LB {offset = -2;}PARALIST RB
			{
				tempnode_paralist = $5;returnvalue=TRUE;returnerr = FALSE;
				tempnode_func = newFuncDecl_Pre(symtablist[lev],$2,$5,FUNC_INT);
				isfuncbody = TRUE;
				offset = 1;
			} 
			Block
				{
					//debug("FuncDef -> INT ...");fflush(stdout);
					if(returnerr==FALSE)yywarning("function needs a value to return",$2);
					$$ = tempnode_func;
					addBodytoFunc($$,$8);
					setLoc($$, (Loc)&(@$));	
				}
			;

PARALIST:	{ $$ = NULL;}
			|PARA
				{	
					$$ = newParalist();
					addLast($$->block->stmts,$1);
					offset--;
					setLoc($$, (Loc)&(@$));	
				}
			|PARALIST COMMA PARA
				{//这里的comma不加会造成冲突
					addLast($1->block->stmts,$3);
					$$ = $1;
					offset--;
					setLoc($$, (Loc)&(@$));	
				}
			;

PARA:		
			INT ident
				{
					$$ = newPara($2,VAR_ID);
					setLoc($$, (Loc)&(@$));	
				}
			|CONST INT ident
				{
					$$ = newPara($3,VAR_ID);
					setLoc($$, (Loc)&(@$));	
				}
			|INT ident LSB RSB
				{
					$$ = newPara($2,VARN_ID);
					setLoc($$, (Loc)&(@$));	
				}
			|CONST INT ident LSB RSB
				{
					$$ = newPara($3,CONSTN_ID);
					setLoc($$, (Loc)&(@$));	
				}
			;

Block:		LLB {
					lev++;
					NEW0(symtablist[lev]);
					if(isfuncbody)
					{
						addParatoSymtab(symtablist[lev],tempnode_paralist);
						isfuncbody=FALSE;					
					}
				}BlockItem {lev--;}RLB
				{
					$3->block->offset = offset;
					$$ = $3;	
					setLoc($$, (Loc)&(@$));									
				}
			|LLB{
					lev++;
					NEW0(symtablist[lev]);
					if(isfuncbody)
					{
						addParatoSymtab(symtablist[lev],tempnode_paralist);
						isfuncbody=FALSE;					
					}
				}RLB
				{
					$$ = newBlock();
					lev--;	
					setLoc($$, (Loc)&(@$));					
				}
			;

BlockItem: 	Decl
				{
					//debug("block->decl");
					$$ = newBlock();
					addLast($$->block->stmts,$1);
					setLoc($$, (Loc)&(@$));					
				}
			|Stmt
				{//debug("blockitem->stmt");	
					$$ = newBlock();
				//debug("blockitem->stmt done");fflush(stdout);
					addLast($$->block->stmts,$1);
					setLoc($$, (Loc)&(@$));	
				
				}
			|BlockItem Decl
				{//debug("blockitem->bllockitem decl");
					addLast($1->block->stmts,$2);
					$$ = $1;
					setLoc($$, (Loc)&(@$));					
				}
			|BlockItem Stmt
				{//debug("blockitem->blockitem stmt");
					addLast($1->block->stmts,$2);
					$$ = $1;
					setLoc($$, (Loc)&(@$));					
				}
			;
Stmt_noSemi:
			LVal ASGN EXP
				{
					$$ = newAssignment($2, $1, $3);
					//dumpSym($$->sym);
					//if($1->sym==NULL)printf("cacacaca\n");	
					//fflush(stdout);
					if($1->sym!=NULL){	
					if($1->kind==KName && $1->sym->type == CONST_ID) yyerror("is a constant and cannot be reassigned a value",$1->sym->name);
					if($1->kind==KArrayQt && $1->arraymem->s->type != VARN_ID)yyerror("is not a variable arrray",$1->arraymem->s->name);}
					setLoc($$, (Loc)&(@$));	
				}
			|CALL
				{
					//debug("Stmt->ident (...)");
					$$ = newCallStmt($1);
					setLoc($$, (Loc)&(@$));					
				}
			|BREAK
				{
					$$ = newBreakStmt();
					if(!lpdepth)yyerror("\"break\" cannot be used outside the loop",NULL);
					setLoc($$,(Loc)&(@$));
				}
			|CONTINUE
				{
					$$ = newContinueStmt();
					if(!lpdepth)yyerror("\"continue\" cannot be used outside the loop",NULL);
					setLoc($$,(Loc)&(@$));
				}
			|RETURN EXP
				{
					if(returnvalue==FALSE)returnerr = FALSE;
					else returnerr = TRUE;
					$$ = newReturnStmt($2);
					setLoc($$,(Loc)&(@$));
					
				}
			|RETURN
				{
					$$ = newReturnStmt(NULL);
					setLoc($$,(Loc)&(@$));
				}
			;
			
Stmt:		
			Block
				{
					$$ = $1;
					setLoc($$, (Loc)&(@$));	
				}
			|IF ELB Cond ERB Stmt 
				{	
					$$ = newIfStmt($3,$5);				
					setLoc($$, (Loc)&(@$));	
				}
			|IF ELB Cond ERB Stmt ELSE Stmt
				{
					$$ = newIfElStmt($3,$5,$7);				
					setLoc($$, (Loc)&(@$));	
				}
			|WHILE ELB Cond ERB {lpdepth++;}Stmt 
				{
					lpdepth--;
					$$ = newWhileStmt($3,$6);	
					setLoc($$, (Loc)&(@$));					
				}
			|SEMICOLON
				{
					$$ = newEmptyStmt();	
					setLoc($$, (Loc)&(@$));				
				}
			
			|Stmt_noSemi SEMICOLON
				{
					//yyerror("';' expected at the end of the  statement",NULL);
					//debug("Stmt -> ... SEMICOLON");
					$$ = $1;
				}
			|Stmt_noSemi
				{
					yyerror("';' expected at the end of the statement",NULL);
					$$ = $1;
				}
			|error SEMICOLON
				{
					yyerror("this is a statement with syntax error",NULL);
					$$ = NULL;
				}
			|error Block
				{
					yyerror("this is a statement with syntax error",NULL);
					$$ = NULL;
				}
			;

CALL:		ident LB RealPARALIST RB
				{
					//debug("Stmt->ident (...)");
					$$ = newCall(Gsymtab,$1,$3);
					setLoc($$, (Loc)&(@$));					
				}
			;
RealPARALIST:
				{$$ = NULL;}
				|EXP
					{
						$$ = newRealPARALIST();
						addLast($$->block->stmts,$1);
						setLoc($$,(Loc)&(@$));
					}
				|RealPARALIST COMMA EXP
					{
						addLast($1->block->stmts,$3);
						$$ = $1;
						setLoc($$,(Loc)&(@$));
					}
			;

LVal:		ident
				{
					$$ = newName(symtablist, $1);
					setLoc($$, (Loc)&(@$));	
							
				 }
			|ident LSB EXP RSB
				{
					$$ = newArrayQt(symtablist, $1,$3);
					setLoc($$, (Loc)&(@$));					
				}
			;

Cond:		odd EXP
				{
					$$ = newPrefixExp($1, $2);	
					setLoc($$, (Loc)&(@$));				
				}
			|EXP RelOp EXP 
				{
					$$ = newInfixExp($2,$1,$3);
					setLoc($$, (Loc)&(@$));	
				}
			;

RelOp:		eql
			|neq
			|lss
			|gtr
			|leq
			|geq
			;

EXP     : NUMBER
	  		{$$ = newNumber($1);setLoc($$, (Loc)&(@$));	}
		|CALL
				{
					$$ = $1;
					setLoc($$, (Loc)&(@$));					
				}
		| LVal
	  		{$$ = $1;}
		| EXP PLUS EXP
	  		{$$ = newInfixExp($2, $1, $3);setLoc($$, (Loc)&(@$));}
		| EXP MINUS EXP
	  		{$$ = newInfixExp($2, $1, $3);setLoc($$, (Loc)&(@$));}
		| EXP MULT EXP
		  	{$$ = newInfixExp($2, $1, $3);setLoc($$, (Loc)&(@$));}
		| EXP DIV EXP
		  	{$$ = newInfixExp($2, $1, $3);setLoc($$, (Loc)&(@$));}
		| EXP MOD EXP
	  		{$$ = newInfixExp($2, $1, $3);setLoc($$, (Loc)&(@$));}
        | MINUS  EXP %prec MINUS
	  		{$$ = newPrefixExp($1, $2); setLoc($$, (Loc)&(@$));}
		| PLUS EXP %prec PLUS
			{$$ = newPrefixExp($1, $2); setLoc($$, (Loc)&(@$));}
        | LB EXP RB
	  		{$$ = newParenExp($2);setLoc($$, (Loc)&(@$));}
        ;

ELB:	{yyerror("'(' expected",NULL);}
		|LB
		;
ERB:	{yyerror("')'expected",NULL);}
		|RB
		;
				
%%

yyerror(char *message,char*name)
{
	errnum++;
	if(!strcmp(message,"syntax error"))
	{
		printf("@line:%-4d column:%-4d %s\n",yylloc.first_line,yylloc.last_column,message);
		return;
	}
	if(!name)
		printf("@line:%-4d column:%-4d error:%s \n",yylloc.first_line,yylloc.last_column,message);
	else
		printf("@line:%-4d column:%-4d error: '%s' %s \n",yylloc.first_line,yylloc.last_column,name,message);
		
}
yywarning(char *message,char *name)
{
	warnnum++;
	if(!name)
		printf("@line:%-4d column:%-4d warning:%s \n",yylloc.first_line,yylloc.last_column,message);
	else
		printf("@line:%-4d column:%-4d warning: '%s' %s \n",yylloc.first_line,yylloc.last_column,name,message);
}
int main(int argc, char *argv[])
{
	int i=0;
	char ch;
    char infilename[38];
	char outfilename[41];
	FILE* infile=NULL;
	switch(argc)
	{
		case 2:
			strcpy(infilename,argv[1]);
			strcpy(outfilename,infilename);
			strcat(outfilename,".out");
			break;
		case 3:
			if(strcmp(argv[1],"-ast"))
			{
				printf("command error\n");
				return 1;
			}			
			strcpy(infilename,argv[2]);
			strcpy(outfilename,infilename);
			strcat(outfilename,".out");
			break;
		case 4:
			if(strcmp(argv[1],"-o"))
			{
				printf("command error\n");
				return 1;
			}
			strcpy(infilename,argv[3]);
			strcpy(outfilename,argv[2]);
			break;
		case 5:
			if(strcmp(argv[1],"-ast")||strcmp(argv[2],"-o"))
			{
				printf("no such command\n");
				return 1;
			}
			strcpy(infilename,argv[4]);
			strcpy(outfilename,argv[3]);
			break;
		default:
			printf("command error\n");
			return 1;
	}
    if((infile=fopen(infilename,"r"))==NULL)
    {
        printf("File %s can't be opened.\n", infilename);
        return 1;
    }
	extern FILE * yyin;
	yyin=infile;
	Gsymtab = newTable();
	symtablist[0]=Gsymtab;
	ast = newAST();
	yyparse();
	//dumpTable(Gsymtab);
	if(!errnum)printf("File analizing has succeeded!\n");
	else printf("File analizing has failed!\n");
	fclose(infile);
	//dumpTable(symtab);
	if(!errnum)
	{
		lev=0;
		freopen(outfilename,"w",stdout);
		if(argc==2||argc==4)
			gencode(ast->root);
		else
			dumpAST(ast->root);
	}
	else printf("error:%d\t warning:%d\n",errnum,warnnum);
	destroyAST(&(ast->root));
	return(0);
}
