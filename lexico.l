%{
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "g-v1.h" /* Este será o header gerado pelo Bison logo em seguida */

extern int yylineno; /* Variável global padrão do Flex para contar linhas */
extern void yyerror(char const *s);
%}

/* Pede ao Flex para manter a contagem de linhas na variável yylineno */
%option yylineno 

/* Criamos um estado exclusivo para tratar o escopo de um comentário */
%x comentario

%%

"/*"                { BEGIN(comentario); }
<comentario>"*/"    { BEGIN(INITIAL); }
<comentario>.|\n    { /* Ignora tudo o que estiver aqui dentro */ }
<comentario><<EOF>> {
                        printf("ERRO: COMENTÁRIO NAO TERMINA %d\n", yylineno);
                        exit(1);
                    }

"principal"         { return PRINCIPAL; }
"int"               { return INT; }
"car"               { return CAR; }
"se"                { return SE; }
"entao"             { return ENTAO; }
"senao"             { return SENAO; }
"fimse"             { return FIMSE; }
"enquanto"          { return ENQUANTO; }
"leia"              { return LEIA; }
"escreva"           { return ESCREVA; }
"novalinha"         { return NOVALINHA; }

"&&"                { return E; } /* Supus '&&' para o E lógico, mas a tabela diz '&'. Precisamos confirmar */
"||"                { return OU; } 
"=="                { return IGUAL; }
"!="                { return DIFERENTE; }
">="                { return MAIORIGUAL; }
"<="                { return MENORIGUAL; }

[ \t\r]+            { /* Ignora espaços em branco e tabulações */ }

.                   {
                        printf("ERRO: CARACTERE INVÁLIDO %d\n", yylineno);
                        exit(1);
                    }

<<EOF>>             { return EOF; } /* O professor exigiu que retorne -1 (EOF) no fim do arquivo [cite: 80] */

%%
int yywrap() { return 1; }
