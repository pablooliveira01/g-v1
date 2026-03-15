CC = gcc-14
FLEX = flex
BISON = bison

g-v1: sintatico.o lexico.o ast.o tabela_simbolos.o semantico.o gerador.o
	$(CC) sintatico.o lexico.o ast.o tabela_simbolos.o semantico.o gerador.o -o g-v1

gerador.o: gerador.c gerador.h
	$(CC) -c gerador.c -o gerador.o

tabela_simbolos.o: tabela_simbolos.c tabela_simbolos.h
	$(CC) -c tabela_simbolos.c -o tabela_simbolos.o

semantico.o: semantico.c semantico.h
	$(CC) -c semantico.c -o semantico.o

ast.o: ast.c ast.h
	$(CC) -c ast.c -o ast.o

sintatico.o: sintatico.c ast.h
	$(CC) -c sintatico.c -o sintatico.o

sintatico.c: g-v1.y
	$(BISON) --defines=g-v1.h -o sintatico.c g-v1.y

lexico.o: lexico.c ast.h
	$(CC) -c lexico.c -o lexico.o

lexico.c: g-v1.l
	$(FLEX) -o lexico.c g-v1.l

clean:
	rm -f *.o g-v1 sintatico.c lexico.c g-v1.h