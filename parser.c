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
#include "lexer.h"
#include "lexerDef.h"
#include "parserDef.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

/* Forward declarations */
static void writeTreeNodeInfo(FILE *destFile, SyntaxTree *node);
static bool isRecoveryBoundary(TOKEN_TYPE tokenType);

/* Tokens that typically indicate statement/block boundaries and help panic-mode recovery */
static bool isRecoveryBoundary(TOKEN_TYPE tokenType) {
  return tokenType == TK_ELSE || tokenType == TK_ENDIF ||
         tokenType == TK_ENDWHILE || tokenType == TK_END ||
         tokenType == TK_MAIN || tokenType == TK_RETURN ||
         tokenType == TK_READ || tokenType == TK_WRITE ||
         tokenType == TK_TYPE || tokenType == TK_CALL;
}

/* Helper function to print a single tree node */
static void writeTreeNodeInfo(FILE *destFile, SyntaxTree *node) {
  /* Print lexeme */
  char *lexeme = (node->nodeData.tokenText != NULL) ? node->nodeData.tokenText : "----";
  fprintf(destFile, "%-30s", lexeme);
  
  /* Print line number */
  fprintf(destFile, "%-30d", node->nodeData.lineNum);

  /* Print token/non-terminal name */
  if (node->nodeData.nodeSymbol.isTerminalSymbol) {
    fprintf(destFile, "%-30s", tokenToString(node->nodeData.nodeSymbol.symbolData.terminalType));
  } else {
    fprintf(destFile, "%-30s", nonTerminalToString(node->nodeData.nodeSymbol.symbolData.nonTerminalType));
  }
  
  /* Print value if number */
  bool isNumericToken = node->nodeData.nodeSymbol.isTerminalSymbol && 
                        (node->nodeData.nodeSymbol.symbolData.terminalType == TK_RNUM ||
                         node->nodeData.nodeSymbol.symbolData.terminalType == TK_NUM);
  if (isNumericToken) {
    fprintf(destFile, "%-30s", (node->nodeData.tokenText != NULL) ? node->nodeData.tokenText : "----");
  } else {
    fprintf(destFile, "%-30s", "----");
  }

  const char *nodeSymbolName = node->nodeData.nodeSymbol.isTerminalSymbol
                                   ? tokenToString(node->nodeData.nodeSymbol.symbolData.terminalType)
                                   : nonTerminalToString(node->nodeData.nodeSymbol.symbolData.nonTerminalType);
  
  /* Print parent and leaf info */
  if (node->parentNode != NULL) {
    fprintf(destFile, "%-30s", nonTerminalToString(node->parentNode->nodeData.nodeSymbol.symbolData.nonTerminalType));
    char *isLeaf = (node->childCount == 0) ? "YES" : "NO";
    fprintf(destFile, "%-30s", isLeaf);
    fprintf(destFile, "%-30s", nodeSymbolName);
    fprintf(destFile, "\n");
  } else {
    fprintf(destFile, "%-30s%-30s%-30s\n", "----", "----", "----");
  }
}

void outputParseTree(FILE *destFile, SyntaxTree *rootNode) {
  /* Handle null cases */
  if (rootNode == NULL || destFile == NULL) {
    return;
  }
  
  /* Print column headers once */
  static int headerPrinted = 1;
  if (headerPrinted) {
    headerPrinted = 0;
    fprintf(destFile, "%-30s%-30s%-30s%-30s%-30s%-30s%-30s\n\n", "lexeme",
            "lineno", "token", "valueIfNumber", "parentNodeSymbol",
            "isLeafNode(yes/no)", "NodeSymbol");
  }
  
  /* Process first child (if exists) */
  if (rootNode->childCount > 0 && rootNode->childNodes[0] != NULL) {
    outputParseTree(destFile, rootNode->childNodes[0]);
  }
  
  /* Print current node data */
  writeTreeNodeInfo(destFile, rootNode);
  
  /* Process remaining children */
  int childIdx = 1;
  while (childIdx < rootNode->childCount) {
    if (rootNode->childNodes[childIdx] != NULL) {
      outputParseTree(destFile, rootNode->childNodes[childIdx]);
    }
    childIdx++;
  }
}

FirstFollowSets deriveFirstFollowSets(GrammarSpec grammar) {
  FirstFollowSets ffSets;

  bool firstCompletionFlags[NONTERMINAL_TOTAL];
  NON_TERMINAL **followDependencies;
  followDependencies =
      (NON_TERMINAL **)malloc(sizeof(NON_TERMINAL *) * NONTERMINAL_TOTAL);

  int dependencyCount[NONTERMINAL_TOTAL];

  /* Initialize all data structures */
  int idx = 0;
  while (idx < NONTERMINAL_TOTAL) {
    ffSets.firstSetSize[idx] = 0;
    ffSets.followSetSize[idx] = 0;
    ffSets.firstContainsEpsilon[idx] = false;
    ffSets.epsilonProductionIdx[idx] = -1;
    firstCompletionFlags[idx] = false;
    dependencyCount[idx] = 0;
    followDependencies[idx] = (NON_TERMINAL *)calloc(PRODUCTION_MAX_LEN, sizeof(NON_TERMINAL));
    idx++;
  }

  /* Compute First Set for all Non Terminals */
  idx = 0;
  while (idx < NONTERMINAL_TOTAL) {
    computeFirstRecursive(grammar, firstCompletionFlags, (NON_TERMINAL)idx, &ffSets);
    idx++;
  }

  /* Initialize Follow set for start symbol '<program>' */
  ffSets.followSet[NT_PROGRAM][0] = DOLLAR;
  ffSets.followSetSize[NT_PROGRAM] = 1;

  /* Compute Follow Set for all Non Terminals */
  idx = 0;
  while (idx < NONTERMINAL_TOTAL) {
    int ruleIdx = 0;
    while (ruleIdx < grammar.productionCount[idx]) {
      processFollowSet(followDependencies, dependencyCount, (NON_TERMINAL)idx, grammar.productionSet[idx][ruleIdx], &ffSets);
      ruleIdx++;
    }
    idx++;
  }

  /* Clear follow set dependency between rules */
  idx = 0;
  while (idx < NONTERMINAL_TOTAL) {
    if (dependencyCount[idx] > 0) {
      resolveFollowDeps(&ffSets, dependencyCount, followDependencies, idx);
    }
    idx++;
  }

  /* Clean up allocated memory */
  idx = 0;
  while (idx < NONTERMINAL_TOTAL) {
    free(followDependencies[idx]);
    idx++;
  }
  free(followDependencies);

  return ffSets;
}

void buildParseTable(ParsingTable *tablePtr, FirstFollowSets ffSets) {
  /* Initialize all entries to -1 (ERROR) */
  int ntIdx = 0;
  while (ntIdx < NONTERMINAL_TOTAL) {
    int tokIdx = 0;
    while (tokIdx < NUM_TOKEN_TYPES) {
      tablePtr->entries[ntIdx][tokIdx] = -1;
      tokIdx++;
    }
    ntIdx++;
  }
  
  /* Populate parse table entries */
  ntIdx = 0;
  while (ntIdx < NONTERMINAL_TOTAL) {
    /* Add entries from first set */
    int fIdx = 0;
    while (fIdx < ffSets.firstSetSize[ntIdx]) {
      TOKEN_TYPE tok = ffSets.firstSet[ntIdx][fIdx];
      tablePtr->entries[ntIdx][tok] = ffSets.productionIndex[ntIdx][fIdx];
      fIdx++;
    }
    
    /* Handle epsilon productions */
    int epsilonIdx = ffSets.epsilonProductionIdx[ntIdx];
    int flIdx = 0;
    
    if (epsilonIdx != -1) {
      while (flIdx < ffSets.followSetSize[ntIdx]) {
        TOKEN_TYPE followTok = ffSets.followSet[ntIdx][flIdx];
        tablePtr->entries[ntIdx][followTok] = epsilonIdx;
        flIdx++;
      }
    } else {
      while (flIdx < ffSets.followSetSize[ntIdx]) {
        TOKEN_TYPE followTok = ffSets.followSet[ntIdx][flIdx];
        if (tablePtr->entries[ntIdx][followTok] == -1) {
          tablePtr->entries[ntIdx][followTok] = -2;
        }
        flIdx++;
      }
    }
    ntIdx++;
  }
}

SyntaxTree *performSyntaxAnalysis(FILE *srcFile, GrammarSpec grammar, FirstFollowSets ffSets, ParsingTable parseTable) {
  /* Declare symbol stack and tree node stack */
  GrammarSymbol *symbolStack[200];
  SyntaxTree *treeStack[200];

  /* Initialize twin buffer */
  DualBufferPtr bufPtr = (DualBufferPtr)malloc(sizeof(DualBuffer));
  int bufIdx = 0;
  while (bufIdx < 2 * BUFF_CAPACITY) {
    bufPtr->storage[bufIdx] = '\0';
    bufIdx++;
  }
  bufPtr->currentPos = 2 * BUFF_CAPACITY - 1;
  bufPtr->currentLine = 1;
  loadBuffer(srcFile, bufPtr);
  bufPtr->currentPos = 0;
  loadBuffer(srcFile, bufPtr);

  /* Initialize keyword lookup table */
  setupKeywordTable();

  /* Initialize stack and error tracking variables */
  int symStackTop = 0;
  int treeStackTop = 0;
  int lookaheadIdx = 0;
  int prevErrorLine = -1;
  int lastMatchedLine = -1;
  TOKEN_TYPE lastMatchedToken = DOLLAR;
  bool hasParseError = false;

  /* Create root node of parse tree */
  SyntaxTree *rootNode = malloc(sizeof(SyntaxTree));
  rootNode->nodeData.nodeSymbol.isTerminalSymbol = false;
  rootNode->nodeData.nodeSymbol.symbolData.nonTerminalType = NT_PROGRAM;
  rootNode->nodeData.lineNum = -1;
  rootNode->nodeData.tokenText = NULL;
  rootNode->nodeData.tokenTextLen = 0;
  rootNode->parentNode = NULL;
  rootNode->childCount = 0;
  int rootChildIdx = 0;
  while (rootChildIdx < PRODUCTION_MAX_LEN) {
    rootNode->childNodes[rootChildIdx] = NULL;
    rootChildIdx++;
  }
  treeStack[treeStackTop] = rootNode;

  /* Construct and push dollar symbol to stack bottom */
  GrammarSymbol *dollarSymbol = (GrammarSymbol *)malloc(sizeof(GrammarSymbol));
  dollarSymbol->isTerminalSymbol = true;
  dollarSymbol->symbolData.terminalType = DOLLAR;
  symbolStack[symStackTop] = dollarSymbol;

  /* Construct and push start symbol <program> */
  GrammarSymbol *programSymbol = (GrammarSymbol *)malloc(sizeof(GrammarSymbol));
  programSymbol->isTerminalSymbol = false;
  programSymbol->symbolData.nonTerminalType = NT_PROGRAM;
  symStackTop++;
  symbolStack[symStackTop] = programSymbol;

  /* Begin parsing with first token */
  TokenInfoPtr currentToken = retrieveToken(srcFile, bufPtr);
  
  while (bufPtr->storage[bufPtr->currentPos] != '\0' && symStackTop >= 0) {
    GrammarSymbol *topSymbol = symbolStack[symStackTop];

    // if top of the stack is a terminal
    if (topSymbol->isTerminalSymbol) {
      if (topSymbol->symbolData.terminalType == DOLLAR && currentToken->tokenCategory == DOLLAR) {
        // if top of stack is dollar and input token is dollar
        // parsing is complete
        break;
      } else if (topSymbol->symbolData.terminalType == currentToken->tokenCategory) {
        // top of stack terminal matches with input token
        // construct and push node to the treeStack
        SyntaxTree *treeNode = treeStack[treeStackTop];
        treeNode->nodeData.nodeSymbol.isTerminalSymbol = true;
        treeNode->nodeData.nodeSymbol.symbolData.terminalType = currentToken->tokenCategory;
        treeNode->nodeData.lineNum = currentToken->lineNumber;
        treeNode->nodeData.tokenText = currentToken->tokenValue;
        treeNode->nodeData.tokenTextLen = currentToken->tokenLength;
        lastMatchedLine = currentToken->lineNumber;
        lastMatchedToken = currentToken->tokenCategory;

        free(topSymbol);
        symbolStack[symStackTop] = NULL;
        symStackTop--;
        treeStackTop--;
        lookaheadIdx++;
        free(currentToken);
        currentToken = retrieveToken(srcFile, bufPtr);
      } else {
        // top of stack terminal does not match with input token
        hasParseError = true;
        if (prevErrorLine == currentToken->lineNumber) {
          free(topSymbol);
          symbolStack[symStackTop] = NULL;
          symStackTop--;
          treeStackTop--;
          continue;
        } else {
          prevErrorLine = currentToken->lineNumber;
          printf(
              "Line %02d: Syntax Error : The token %s for lexeme %s does not "
              "match with the expected token %s\n",
              currentToken->lineNumber, tokenToString(currentToken->tokenCategory), currentToken->tokenValue,
              tokenToString(topSymbol->symbolData.terminalType));
          free(topSymbol);
          symbolStack[symStackTop] = NULL;
          symStackTop--;
          treeStackTop--;
        }
      }
    } else {
      // if top of stack is a non terminal
      NON_TERMINAL ntSymbol = topSymbol->symbolData.nonTerminalType;
      int ruleIndex = parseTable.entries[ntSymbol][currentToken->tokenCategory];
      if (ruleIndex == -1) {
        // if entry of parse table is -1 (ERROR)
        hasParseError = true;
        if (prevErrorLine == currentToken->lineNumber) {
          if (isRecoveryBoundary(currentToken->tokenCategory)) {
            free(topSymbol);
            symbolStack[symStackTop] = NULL;
            symStackTop--;
            treeStackTop--;
          } else {
            lookaheadIdx++;
            free(currentToken);
            currentToken = retrieveToken(srcFile, bufPtr);
          }
          continue;
        } else {
          prevErrorLine = currentToken->lineNumber;
          int reportLine = currentToken->lineNumber;
          TOKEN_TYPE reportToken = currentToken->tokenCategory;
          const char *reportLexeme = currentToken->tokenValue;
          if (ntSymbol == NT_OPTION_SINGLE_CONSTRUCTED &&
              currentToken->tokenCategory == TK_ELSE && reportLine > 1) {
            reportLine = reportLine - 1;
          } else if (ntSymbol == NT_OTHERSTMTS &&
                     currentToken->tokenCategory == TK_END &&
                     lastMatchedToken == TK_ENDIF && lastMatchedLine > 0) {
            reportLine = lastMatchedLine;
            reportToken = TK_ENDIF;
            reportLexeme = "endif";
          }
          printf("Line %02d: Syntax Error : Invalid token %s encountered with "
                 "value %s stack top %s\n",
                 reportLine, tokenToString(reportToken), reportLexeme, nonTerminalToString(ntSymbol));
          if (isRecoveryBoundary(currentToken->tokenCategory)) {
            free(topSymbol);
            symbolStack[symStackTop] = NULL;
            symStackTop--;
            treeStackTop--;
          } else {
            lookaheadIdx++;
            free(currentToken);
            currentToken = retrieveToken(srcFile, bufPtr);
          }
        }
      } else if (ruleIndex == -2) {
        // if entry of parse table is -2 (SYN)
        hasParseError = true;
        if (prevErrorLine == currentToken->lineNumber) {
          free(topSymbol);
          symbolStack[symStackTop] = NULL;
          symStackTop--;
          treeStackTop--;
          continue;
        } else {
          prevErrorLine = currentToken->lineNumber;
          printf("Line %02d: Syntax Error : Invalid token %s encountered with "
                 "value %s stack top %s\n",
                 currentToken->lineNumber, tokenToString(currentToken->tokenCategory), currentToken->tokenValue, nonTerminalToString(ntSymbol));
          free(topSymbol);
          symbolStack[symStackTop] = NULL;
          symStackTop--;
          treeStackTop--;
        }
      } else {
        // if entry of parse table is a valid rule
        // remove non terminal from stack
        ProductionRule production = grammar.productionSet[ntSymbol][ruleIndex];
        SyntaxTree *treeNode = treeStack[treeStackTop];
        treeStackTop--;
        free(topSymbol);
        symbolStack[symStackTop] = NULL;
        symStackTop--;

        if (grammar.derivesEpsilon[ntSymbol] && ruleIndex == grammar.productionCount[ntSymbol]) {
          // if the rule is an epsillon rule
          // make epsillon node and add to children
          treeNode->childCount = 1;
          SyntaxTree *childTree = (SyntaxTree *)malloc(sizeof(SyntaxTree));
          childTree->nodeData.nodeSymbol.isTerminalSymbol = true;
          childTree->nodeData.nodeSymbol.symbolData.terminalType = EPSILLON;
          childTree->nodeData.lineNum = -1;
          childTree->nodeData.tokenText = NULL;
          childTree->nodeData.tokenTextLen = 0;
          childTree->parentNode = treeNode;
          childTree->childCount = 0;
          int epsChildIdx = 0;
          while (epsChildIdx < PRODUCTION_MAX_LEN) {
            childTree->childNodes[epsChildIdx] = NULL;
            epsChildIdx++;
          }
          treeNode->childNodes[0] = childTree;
        } else {
          // if the rule is not a non epsillon rule
          // make child nodes for each element in the rule

          treeNode->childCount = production.symbolCount;

          // push child nodes on the treeStack in reverse order
          for (int elemIdx = production.symbolCount - 1; elemIdx >= 0; elemIdx--) {
            SyntaxTree *childTree = (SyntaxTree *)malloc(sizeof(SyntaxTree));

            childTree->nodeData.nodeSymbol.isTerminalSymbol = production.symbolList[elemIdx].isTerminalSymbol;
            childTree->nodeData.lineNum = -1;
            childTree->nodeData.tokenText = NULL;
            childTree->nodeData.tokenTextLen = 0;

            if (production.symbolList[elemIdx].isTerminalSymbol) {
              childTree->nodeData.nodeSymbol.symbolData.terminalType = production.symbolList[elemIdx].symbolData.terminalType;
            } else {
              childTree->nodeData.nodeSymbol.symbolData.nonTerminalType = production.symbolList[elemIdx].symbolData.nonTerminalType;
            }

            childTree->parentNode = treeNode;
            childTree->childCount = 0;
            int childIdxInit = 0;
            while (childIdxInit < PRODUCTION_MAX_LEN) {
              childTree->childNodes[childIdxInit] = NULL;
              childIdxInit++;
            }
            treeNode->childNodes[elemIdx] = childTree;

            treeStack[++treeStackTop] = childTree;
            GrammarSymbol *symElem =
                (GrammarSymbol *)malloc(sizeof(GrammarSymbol));
            symElem->isTerminalSymbol = production.symbolList[elemIdx].isTerminalSymbol;
            if (symElem->isTerminalSymbol) {
              symElem->symbolData.terminalType = production.symbolList[elemIdx].symbolData.terminalType;
            } else {
              symElem->symbolData.nonTerminalType = production.symbolList[elemIdx].symbolData.nonTerminalType;
            }

            symbolStack[++symStackTop] = symElem;
          }
        }
      }
    }
  }
  /* Verify parsing completed correctly */
  bool stackEmpty = (symStackTop == 0 && symbolStack[symStackTop]->symbolData.terminalType == DOLLAR);
  bool inputConsumed = (currentToken->tokenCategory == DOLLAR);
  
  if (!stackEmpty) {
    hasParseError = true;
    printf("Syntax Error : Tokens parsed but stack not empty\n");
  } else if (!inputConsumed) {
    hasParseError = true;
    printf("Syntax Error : Stack empty but tokens not parsed\n");
  }
  
  /* Free memory used by twin buffer */
  free(bufPtr);

  /* Print compilation result */
  if (hasParseError) {
    printf("COMPILATION FAILED\n");
  } else {
    printf("COMPILATION SUCCESS!\n");
  }

  /* Clear symbol stack (memory cleanup) */
  while (symStackTop >= 0) {
    GrammarSymbol *stackElem = symbolStack[symStackTop];
    if (stackElem != NULL) {
      free(stackElem);
    }
    symStackTop--;
  }

  return rootNode;
}
