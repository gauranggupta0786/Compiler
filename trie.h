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
#ifndef TRIE
#define TRIE

#include "lexerDef.h"
#define CHAR_SET_SIZE 26

typedef struct TrieNode
{
    struct TrieNode* childLinks[CHAR_SET_SIZE];
    TOKEN_TYPE storedTokenType;
} TrieNode;
typedef TrieNode* TrieNodePtr;

TrieNodePtr allocateTrieNode();
void insertKeyword(char* keyStr, TrieNodePtr rootPtr, TOKEN_TYPE tokenType);
TOKEN_TYPE searchKeyword(char* keyStr, TrieNodePtr rootPtr);

#endif