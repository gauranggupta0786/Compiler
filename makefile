#   Group Number: 22
#   NAME                 : ID
#   Archit Garg  :      : 2022B2A71602PP 
#   Aadit Shah          : 2022B3A70612P 
#   Rishant Sharma  : 2022B5A70814P 
#   Rishab Chetal     : 2023A7PS0015P 
#   Gaurang Gupta  : 2023A7PS0648P
#   Ansh Bansal       : 2023A7PS0645P 

exec: lexer.o trie.o string.o utils.o parser.o driver.o
	gcc -o stage1exe driver.o lexer.o parser.o string.o trie.o utils.o
lexer.o: lexer.c lexer.h lexerDef.h
	gcc -c lexer.c

trie.o: trie.c trie.h
	gcc -c trie.c

string.o: string.c string.h
	gcc -c string.c

utils.o: utils.c utils.h
	gcc -c utils.c

parser.o: parser.c parserDef.h
	gcc -c parser.c

driver.o: driver.c
	gcc -c driver.c

run_parser: lexer.o trie.o string.o parser.o utils.o
	gcc -o run_parser lexer.o trie.o string.o parser.o utils.o

run: run_parser
	./run_parser

run_lexer: lexer.o trie.o string.o
	gcc -o run_lexer lexer.o trie.o string.o
	./run_lexer

clean:
	rm *.o run_parser run_lexer exec
