/*
 * C1.lex : Scanner for a simple
 *            expression parser.
 */

%{
#include <common.h>
#include "c1c.tab.h"
#include <stdio.h>
#include <string.h>
int errchar=0;
int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
    yycolumn += yyleng;
%}
%option yylineno
delima	   	[ \t]
ws	   		{delima}+
letter     	[a-zA-Z]
hexletter  	[a-fA-F]
digit      	[0-9]
num_dec    	([1-9]{digit}*|"0")
num_oct    	"0"{digit}*
num_hex    	("0x"|"0X")({digit}|{hexletter})+
numbers    	{num_dec}|{num_oct}|{num_hex}
id	   		{letter}+({letter}|{digit})*
other1     	[^"*"]
other2     	[^"/"]
comment    	"/*"{other1}*("*"+{other2}{other1}*)*"*"*"*/"
other3		[^'\n']
commentline	"//"{other3}*
%%

{comment}  		{}
{commentline}	{}
{ws}	   	 

mod 	   	{
				yylval.ival = OP_MOD;
				return MOD;
	   		}

const      	{	
				 
				return CONST;
	   		}	
void	   	{
				 		
				return VOID;
	   		}

int        	{
				 
				return INT;
	   		}

if 	   		{
				 
				return IF;
	   		}
else		{
				 
				return ELSE;
			}

odd        	{
				yylval.ival = OP_ODD;		
				return odd;
	  		}

break		{
				return BREAK;
			}
continue	{
				return CONTINUE;
			}
while	   	{
				 
				return WHILE;  
	   		}
return 		{
				return RETURN;
			}

{id}		{
				yylval.name = malloc(yyleng+1);
	     		yylval.name[yyleng] = '\0';
             	strncpy(yylval.name, yytext, yyleng);
				return ident;    	   
	   		}
{num_dec}  	{
	     		yylval.ival = atol(yytext); 
             	return NUMBER;
           	}
{num_oct}  	{
				 
				return NUMBER;
	   		}
{num_hex}  	{
				 
				return NUMBER;
	   		}
"="	   		{
				yylval.ival = OP_ASGN;		
				return ASGN;
			}
"<="       	{
				yylval.ival = OP_LEQ; 
				return leq;}
"!="	   	{
				yylval.ival = OP_NEQ;		
				return neq;
			}
"<"	   		{
				yylval.ival = OP_LSS; 		
				return lss;
			}
">="	   	{
				yylval.ival = OP_GEQ; 
				return geq;
			}
">"	   		{	
				yylval.ival = OP_GTR; 
				return gtr;
			}
"+"        	{
				yylval.ival = OP_PLUS; 
				return PLUS;
			}

"-"	   		{
				yylval.ival = OP_MINUS; 
				return MINUS;
			}
"*" 	   	{
				yylval.ival = OP_MULT; 
				return MULT;
			}
"/"        	{
				yylval.ival = OP_DIV; 		
				return DIV;
			}
"("	   		{	
				 
				return LB;
			}
")"	   		{
				 
				return RB;
			}

"=="	   	{
				yylval.ival = OP_EQL;	 
				return eql;
			}
","        	{
				 
				return COMMA;
			}
";"	   		{
				 
				return SEMICOLON;
			}

"%"        	{
				yylval.ival = OP_MOD; 
				return MOD;
	   		}
"["	   		{
				 
				return LSB;
	   		}
"]"	   		{
				 
				return RSB;
			}
"{"			{
				 
				return LLB;
			}
"}"			{
				 
				return RLB;
			}
"\n"		{yycolumn = 1;}
.			{yyerror("illegal string",NULL);yylval.name=(char*)malloc(15);sprintf(yylval.name,"%d",errchar++);return ident;}
%%


