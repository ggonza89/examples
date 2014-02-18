%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* external function prototypes */
extern int yylex();
extern int initLex(int ,  char **);



/* external global variables */

extern int		yydebug;
extern int		yylineno;


/* function prototypes */
void	yyerror(const char *);

/* global variables */

%}

/* YYSTYPE */

/* terminals */
/* Start adding token names here */
/* Your token names must match Project 1 */
/* The file cmparser.tab.h was gets generated here */

%token TOK_ERROR
%token TOK_ELSE
%token TOK_IF
%token TOK_RETURN
%token TOK_VOID
%token TOK_INT
%token TOK_WHILE
%token TOK_PLUS
%token TOK_MINUS
%token TOK_MULT
%token TOK_DIV
%token TOK_LT
%token TOK_LE
%token TOK_GT
%token TOK_GE
%token TOK_EQ
%token TOK_NE
%token TOK_ASSIGN
%token TOK_SEMI
%token TOK_COMMA
%token TOK_LPAREN
%token TOK_RPAREN
%token TOK_LSQ
%token TOK_RSQ
%token TOK_LBRACE
%token TOK_RBRACE
%token TOK_NUM
%token TOK_ID

/* associativity and precedence */
/* specify operator precedence (taken care of by grammar) and associatity here --uncomment */

/*%right '='
%left '!=' '=='
%nonassoc '>=' '<=' '>' '<'
%left '-' '+'
%left '/' '*'*/

%nonassoc error

/* Begin your grammar specification here */
%%


Start : Declarations /* put your RHS for this rule here */
Declarations : Func_declarations | Var_declarations Func_declarations;

//Variable declarations
Var_declarations : Var_declaration | Var_declaration Var_declarations;
Var_declaration : Type_specifier TOK_ID TOK_SEMI | Array_declaration;
Array_declaration : Type_specifier TOK_ID TOK_LSQ TOK_NUM TOK_RSQ TOK_SEMI;
Type_specifier : TOK_INT | TOK_VOID;

//function declarations
Func_declarations : Func_declaration | Func_declaration Func_declarations;
Func_declaration : Type_specifier TOK_ID TOK_LPAREN Params TOK_RPAREN Compound_stmt;

//paramaters
Params : Param_list | TOK_VOID;
Param_list : Param_list TOK_COMMA Param | Param;
Param : Type_specifier TOK_ID | Type_specifier TOK_ID TOK_LSQ TOK_RSQ;

//statements
Compound_stmt : TOK_LBRACE Local_declarations Statements TOK_RBRACE;
Local_declarations : Var_declarations;
Statements : Statement Statements | Statement;
Statement : Expression_stmt;

Expression_stmt : Expression TOK_SEMI | TOK_SEMI;
Expression : Var TOK_ASSIGN Expression | Simple_expression;
Simple_expression : Additive_expression;
Additive_expression : Additive_expression Add_op Term | Term;

Term : Term Mult_op Factor | Factor;
Mult_op : TOK_MULT | TOK_DIV;
Add_op : TOK_PLUS | TOK_MINUS;
Factor : TOK_LPAREN Simple_expression TOK_RPAREN | Var | TOK_NUM | Call;
Var : TOK_ID | TOK_ID TOK_LSQ Simple_expression TOK_RSQ;

Call : TOK_ID TOK_LPAREN Args TOK_RPAREN;
Args : /* empty */ | Args_list ;
Args_list : Args_list TOK_COMMA Simple_expression | Simple_expression;

/* note that the rule ends with a semicolon */
%%
void yyerror (char const *s) {
       fprintf (stderr, "Line %d: %s\n", yylineno, s);
}

int main(int argc, char **argv){

	initLex(argc,argv);

#ifdef YYLLEXER
   while (gettok() !=0) ; //gettok returns 0 on EOF
    return;
#else
    yyparse();

#endif

}
