%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "g-v1.h"

extern int yylineno;
extern void yyerror(char const *s);
%}

%option yylineno
%option noyywrap

%x COMENTARIO

DIGITO      [0-9]
LETRA       [a-zA-Z]
ID          ({LETRA}|_)({LETRA}|{DIGITO}|_)*
INTCONST    [+-]?{DIGITO}+
CARCONST    '([^'\\]|\\.)'
STR         \"[^\"\n]*\"
STR_ERRO    \"[^\"\n]*\n

%%

"/*"                { BEGIN(COMENTARIO); }
<COMENTARIO>"*/"    { BEGIN(INITIAL); }
<COMENTARIO>.       { }
<COMENTARIO>\n      { }
<COMENTARIO><<EOF>> { printf("ERRO: COMENTÁRIO NAO TERMINA %d\n", yylineno); exit(1); }

"principal"         { return PRINCIPAL; }
"int"               { return INT; }
"car"               { return CAR; }
"leia"              { return LEIA; }
"escreva"           { return ESCREVA; }
"novalinha"         { return NOVALINHA; }
"se"                { return SE; }
"entao"             { return ENTAO; }
"senao"             { return SENAO; }
"fimse"             { return FIMSE; }
"enquanto"          { return ENQUANTO; }

"||"                { yylval.str = strdup(yytext); return OU; }
"&&"                { yylval.str = strdup(yytext); return E; }
"=="                { yylval.str = strdup(yytext); return IGUAL; }
"!="                { yylval.str = strdup(yytext); return DIFERENTE; }
">="                { yylval.str = strdup(yytext); return MAIORIGUAL; }
"<="                { yylval.str = strdup(yytext); return MENORIGUAL; }

"+"|"-"|"*"|"/"|"<"|">"|"="|";"|":"|","|"!"|"("|")"|"{"|"}" { return yytext[0]; }

{INTCONST}          { yylval.str = strdup(yytext); return INTCONST; }
{CARCONST}          { yylval.str = strdup(yytext); return CARCONST; }
{ID}                { yylval.str = strdup(yytext); return IDENTIFICADOR; }
{STR}               { yylval.str = strdup(yytext); return CADEIACARACTERES; }

{STR_ERRO}          { printf("ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA %d\n", yylineno - 1); exit(1); }

[ \t\r\n]+          { }
.                   { printf("ERRO: CARACTERE INVÁLIDO %d\n", yylineno); exit(1); }
<<EOF>>             { return EOF; }
%%