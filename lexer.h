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
#ifndef LEXER_HEADER
#define LEXER_HEADER

#include "lexerDef.h"
#include <stdio.h>

void initializeInputStream(FILE *srcFile);
TokenInfoPtr fetchNextToken(FILE *srcFile, DualBufferPtr bufferPtr);
void stripComments(char *srcFilePath);
TokenInfoPtr retrieveToken(FILE *srcFile, DualBufferPtr bufferPtr);
char *tokenToString(TOKEN_TYPE tokType);
void loadBuffer(FILE *srcFile, DualBufferPtr bufferPtr);
void setupKeywordTable();
bool validateTokenError(TokenInfoPtr tokPtr);

#endif
