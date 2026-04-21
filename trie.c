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
#include "trie.h"
#include <stdlib.h>

/* Forward declaration */
static void initializeChildLinks(TrieNodePtr nodePtr);

TrieNodePtr allocateTrieNode() {
    TrieNodePtr nodePtr = (TrieNodePtr)malloc(sizeof(TrieNode));
    initializeChildLinks(nodePtr);
    nodePtr->storedTokenType = TK_FIELDID;
    return nodePtr;
}

/* searchKeyword function - finds token type for a given key */
TOKEN_TYPE searchKeyword(char* keyStr, TrieNodePtr rootPtr) {
    TrieNodePtr currentNode = rootPtr;
    int charIdx = 0;
    
    while (keyStr[charIdx] != '\0') {
        int childIndex = keyStr[charIdx] - 'a';
        
        if (currentNode->childLinks[childIndex] == NULL) {
            return TK_FIELDID;
        }
        
        currentNode = currentNode->childLinks[childIndex];
        charIdx++;
    }
    
    return currentNode->storedTokenType;
}

/* insertKeyword function - adds a keyword to the trie */
void insertKeyword(char* keyStr, TrieNodePtr rootPtr, TOKEN_TYPE tokenType) {
    TrieNodePtr currentNode = rootPtr;
    int charIdx = 0;
    
    while (keyStr[charIdx] != '\0') {
        int childIndex = keyStr[charIdx] - 'a';
        
        if (currentNode->childLinks[childIndex] == NULL) {
            currentNode->childLinks[childIndex] = allocateTrieNode();
        }
        
        currentNode = currentNode->childLinks[childIndex];
        charIdx++;
    }
    
    currentNode->storedTokenType = tokenType;
}

/* Initialize all child pointers to NULL - placed after functions that use it */
static void initializeChildLinks(TrieNodePtr nodePtr) {
    int idx = 0;
    while (idx < CHAR_SET_SIZE) {
        nodePtr->childLinks[idx] = NULL;
        idx++;
    }
}