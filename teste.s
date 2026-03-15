.text
.globl main
main:
  move $fp, $sp # Inicia o Frame Pointer
  addi $sp, $sp, -4 # Aloca espaco para a variavel 'x'
  addi $sp, $sp, -4 # Aloca espaco para a variavel 'letra'
  li $t0, 10
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  addi $sp, $sp, 4
  lw $t0, 0($sp)
  sw $t0, -4($fp) # Salva valor em x
  lw $t0, -4($fp) # Carrega x
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  li $t0, 5
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  addi $sp, $sp, 4
  lw $t1, 0($sp)
  addi $sp, $sp, 4
  lw $t0, 0($sp)
  sgt $t0, $t0, $t1
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  addi $sp, $sp, 4
  lw $t0, 0($sp)
  beq $t0, $zero, L0 # Pula se for Falso
.data
str_0: .asciiz "Tudo funcionando!"
.text
  la $a0, str_0
  li $v0, 4
  syscall
L0:
  li $v0, 10 # Syscall para Encerrar
  syscall