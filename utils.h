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
#ifndef UTILS_HEADER
#define UTILS_HEADER
#include "parserDef.h"

GrammarSpec loadGrammarRules();
void mergeFirstSets(NON_TERMINAL nt1, NON_TERMINAL nt2, FirstFollowSets *ffSets);
char *nonTerminalToString(NON_TERMINAL ntSymbol);
void computeFirstRecursive(GrammarSpec grammar, bool *completionFlags, 
                           NON_TERMINAL ntSymbol, FirstFollowSets *ffSets);
void appendToFollowSet(TOKEN_TYPE tokenType, NON_TERMINAL ntSymbol, FirstFollowSets *ffSets);
void appendFollowDependency(int *depCounts, NON_TERMINAL **followDeps,
                            NON_TERMINAL depNt, NON_TERMINAL ntSymbol);
void processFollowSet(NON_TERMINAL **followDeps, int *depCounts, NON_TERMINAL lhsSymbol,
                      ProductionRule production, FirstFollowSets *ffSets);
void resolveFollowDeps(FirstFollowSets *ffSets, int *depCounts,
                       NON_TERMINAL **followDeps, NON_TERMINAL ntSymbol);
#endif
