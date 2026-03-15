#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gerador.h"
#include "tabela_simbolos.h"

int deslocamento = 0;
int cont_label = 0;
int cont_str = 0;

/* Resolve contas matemáticas usando a pilha (Stack Machine) */
void gerar_expr(NoAST *no) {
    if (!no) return;
    
    if (no->tipo == NODE_INT) {
        printf("  li $t0, %s\n", no->lexema);
        printf("  sw $t0, 0($sp)\n  addi $sp, $sp, -4\n");
    } 
    else if (no->tipo == NODE_ID) {
        Simbolo *s = pesquisar_simbolo(no->lexema);
        printf("  lw $t0, %d($fp) # Carrega %s\n", s->offset, no->lexema);
        printf("  sw $t0, 0($sp)\n  addi $sp, $sp, -4\n");
    } 
    else if (no->tipo == NODE_OP_BINARIA) {
        gerar_expr(no->esq);
        gerar_expr(no->dir);
        
        printf("  addi $sp, $sp, 4\n  lw $t1, 0($sp)\n"); /* Lado Direito */
        printf("  addi $sp, $sp, 4\n  lw $t0, 0($sp)\n"); /* Lado Esquerdo */
        
        if (strcmp(no->lexema, "+") == 0) printf("  add $t0, $t0, $t1\n");
        else if (strcmp(no->lexema, "-") == 0) printf("  sub $t0, $t0, $t1\n");
        else if (strcmp(no->lexema, "*") == 0) printf("  mul $t0, $t0, $t1\n");
        else if (strcmp(no->lexema, "/") == 0) printf("  div $t0, $t1\n  mflo $t0\n");
        else if (strcmp(no->lexema, ">") == 0) printf("  sgt $t0, $t0, $t1\n");
        else if (strcmp(no->lexema, "<") == 0) printf("  slt $t0, $t0, $t1\n");
        
        printf("  sw $t0, 0($sp)\n  addi $sp, $sp, -4\n");
    }
}

void gerar_codigo_ast(NoAST *no) {
    if (!no) return;
    
    switch(no->tipo) {
        case NODE_PROGRAMA:
            printf(".text\n.globl main\nmain:\n");
            printf("  move $fp, $sp # Inicia o Frame Pointer\n");
            gerar_codigo_ast(no->esq);
            printf("  li $v0, 10 # Syscall para Encerrar\n  syscall\n");
            break;
            
        case NODE_BLOCO:
            empilhar_escopo();
            gerar_codigo_ast(no->esq); /* Percorre Variáveis */
            gerar_codigo_ast(no->dir); /* Percorre Comandos */
            remover_escopo();
            break;
            
        case NODE_DECL_VAR:
            /* Reserva espaço para a variável na memória do MIPS e anota a posição na tabela */
            deslocamento -= 4;
            Simbolo *s = inserir_simbolo(no->lexema, "int", no->linha);
            if(s) s->offset = deslocamento;
            printf("  addi $sp, $sp, -4 # Aloca espaco para a variavel '%s'\n", no->lexema);
            break;
            
        case NODE_ATRIB:
            gerar_expr(no->dir);
            printf("  addi $sp, $sp, 4\n  lw $t0, 0($sp)\n");
            s = pesquisar_simbolo(no->esq->lexema);
            printf("  sw $t0, %d($fp) # Salva valor em %s\n", s->offset, no->esq->lexema);
            break;
            
        case NODE_ESCREVA:
            if (no->esq->tipo == NODE_STR) {
                /* MIPS aceita intercalar .data e .text */
                printf(".data\nstr_%d: .asciiz %s\n.text\n", cont_str, no->esq->lexema);
                printf("  la $a0, str_%d\n  li $v0, 4\n  syscall\n", cont_str++);
            } else {
                gerar_expr(no->esq);
                printf("  addi $sp, $sp, 4\n  lw $a0, 0($sp)\n  li $v0, 1\n  syscall\n");
            }
            break;
            
        case NODE_SE: {
            int l_fim = cont_label++;
            gerar_expr(no->condicao);
            printf("  addi $sp, $sp, 4\n  lw $t0, 0($sp)\n");
            printf("  beq $t0, $zero, L%d # Pula se for Falso\n", l_fim);
            gerar_codigo_ast(no->esq);
            printf("L%d:\n", l_fim);
            break;
        }
        default: break;
    }
    if (no->prox) gerar_codigo_ast(no->prox);
}

void gerar_codigo(NoAST *raiz) {
    iniciar_pilha();
    deslocamento = 0;
    printf("\n# ==================================\n");
    printf("# --- CODIGO ASSEMBLY MIPS GERADO --\n");
    printf("# ==================================\n");
    gerar_codigo_ast(raiz);
}