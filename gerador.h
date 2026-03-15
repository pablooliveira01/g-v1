#ifndef GERADOR_H
#define GERADOR_H

#include "ast.h"

/* Inicia a tradução da AST para código MIPS */
void gerar_codigo(NoAST *raiz);

#endif