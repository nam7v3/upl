%option yylineno
%{
#include <stdio.h>
static int yycolno = 1;
%}
ALPHA   [a-zA-Z]
DIGIT   [0-9]
ALNUM   [a-zA-Z0-9]
WS      [ \t\n]

%%
"//".*        ;
"/*"([^*]|\*+[^*/])*\*"/"        ; 
"\n"               { yycolno = 1; };
{WS}+         ; /* Ignore whitespace */
\+                 { printf("ADD(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }
\*                 { printf("MUL(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }
";"                { printf("SEMICOLON(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }
"("                { printf("OPAREN(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }              
")"                { printf("CPAREN(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }              
"{"                { printf("OCURLY(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }              
"}"                { printf("CCURLY(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }              
">="               { printf("GTEQUAL(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }            
">"                { printf("GT(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }                  
"=="               { printf("DEQUAL(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }             
"="                { printf("EQUAL(line=%d, col=%d)\n", yylineno, yycolno); yycolno += yyleng; }               
{DIGIT}+           { printf("NUM(line=%d, col=%d, value=%s)\n", yylineno, yycolno); yycolno += yyleng; }
"bool"             { printf("BOOL(line=%d, col=%d)\n", yylineno, yycolno)   ; yycolno += yyleng; }
"int"              { printf("INT(line=%d, col=%d)\n", yylineno, yycolno)    ; yycolno += yyleng; }
"if"               { printf("IF(line=%d, col=%d)\n", yylineno, yycolno)     ; yycolno += yyleng; }
"then"             { printf("THEN(line=%d, col=%d)\n", yylineno, yycolno)   ; yycolno += yyleng; }
"else"             { printf("ELSE(line=%d, col=%d)\n", yylineno, yycolno)   ; yycolno += yyleng; }
"do"               { printf("DO(line=%d, col=%d)\n", yylineno, yycolno)     ; yycolno += yyleng; }
"while"            { printf("WHILE(line=%d, col=%d)\n", yylineno, yycolno)  ; yycolno += yyleng; }
"begin"            { printf("BEGIN(line=%d, col=%d)\n", yylineno, yycolno)  ; yycolno += yyleng; }
"end"              { printf("END(line=%d, col=%d)\n", yylineno, yycolno)    ; yycolno += yyleng; }
"print"            { printf("PRINT(line=%d, col=%d)\n", yylineno, yycolno)    ; yycolno += yyleng; }
{ALPHA}+{DIGIT}*   { printf("IDENTIFIER(line=%d, col=%d, name=\"%s\")\n", yylineno, yycolno, yytext); yycolno += yyleng; }
{ALPHA}+{ALNUM}*   { printf("IDENTIFIER ERROR: UPL only accpept identifier that start with character and end with number, get %s", yytext); yycolno += yyleng; }
.                  { printf("UNKNOWN TOKEN(%s)\n", yytext); yycolno += yyleng; }
%%

int yywrap() {
    return 1;
}
