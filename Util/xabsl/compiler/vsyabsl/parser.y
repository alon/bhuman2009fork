%{
#include <stdio.h> 

#define YYMAXDEPTH 10000
#define YYINITDEPTH 10000

#include "service.h"

ScopeStorage g_storage = StorageStatic;;

%}
%locations

%token IDENTIFIER NUMBER 
%token KEYWORD PROCEDURE
%token ',' ';' '(' ')' '{' '}' '=' 
%token '+' '-' '*' '/' '!' '&' '|' '^'
%token EQ NEQ GT GTE LT LTE AMPAMP BARBAR 
%token LEX_WHITE LEX_COMMENT LEX_LINE_COMMENT

%token '.'

%left '*' '/'
%left '+' '-'
%%

Program:
    ApiDeclarations
    ;


ApiDeclarations:
	ApiDeclaration ApiDeclarations
	|
	;

ApiDeclaration:
	PROCEDURE
	{ g_service->addScope( $1, $1, ScopeProcedure , AccessPublic, g_storage, $1, $1, $1);	}                           
	|
	IDENTIFIER
	{ g_service->addScope( $1, $1, ScopeVariable , AccessPublic, g_storage, $1, $1, $1);}                           
	|
	KEYWORD
	{ g_service->addScope( $1, $1, ScopeInterface , AccessPublic, g_storage, $1, $1, $1);}
	;

    
%%
