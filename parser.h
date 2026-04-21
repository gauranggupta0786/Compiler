/*
  Group Number: 22
  NAME                 : ID
  Archit Garg          : 2022B2A71602PP 
  Aadit Shah           : 2022B3A70612P 
  Rishant Sharma       : 2022B5A70814P 
  Rishab Chetal        : 2023A7PS0015P 
  Gaurang Gupta        : 2023A7PS0648P
  Ansh Bansal          : 2023A7PS0645P 
*/
#ifndef PARSER_HEADER
#define PARSER_HEADER
#include "parserDef.h"
#include <stdio.h>

void buildParseTable(ParsingTable *tablePtr, FirstFollowSets ffSets);

SyntaxTree *performSyntaxAnalysis(FILE *srcFile, GrammarSpec grammar, FirstFollowSets ffSets, ParsingTable parseTable);

void outputParseTree(FILE *destFile, SyntaxTree *rootNode);

FirstFollowSets deriveFirstFollowSets(GrammarSpec grammar);
#endif
