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
#ifndef PARSER_DEF
#define PARSER_DEF

#include "lexerDef.h"

#define NONTERMINAL_TOTAL 53
#define PRODUCTION_MAX_LEN 15
typedef enum {
  NT_PROGRAM,
  NT_MAINFUNCTION,
  NT_OTHERFUNCTIONS,
  NT_FUNCTION,
  NT_INPUT_PAR,
  NT_OUTPUT_PAR,
  NT_PARAMETER_LIST,
  NT_DATATYPE,
  NT_PRIMITIVEDATATYPE,
  NT_CONSTRUCTEDDATATYPE,
  NT_REMAINING_LIST,
  NT_STMTS,
  NT_TYPEDEFINITIONS,
  NT_ACTUALORREDEFINED,
  NT_TYPEDEFINITION,
  NT_FIELDDEFINITIONS,
  NT_FIELDDEFINITION,
  NT_FIELDTYPE,
  NT_MOREFIELDS,
  NT_DECLATRATIONS,
  NT_DECLATRATION,
  NT_GLOBAL_OR_NOT,
  NT_OTHERSTMTS,
  NT_STMT,
  NT_ASSIGNMENTSTMT,
  NT_SINGLEORRECID,
  NT_OPTION_SINGLE_CONSTRUCTED,
  NT_ONEEXPANSION,
  NT_MOREEXPANSIONS,
  NT_FUNCALLSTMT,
  NT_OUTPUTPARAMETERS,
  NT_INPUTPARAMETERS,
  NT_ITERATIVESTMT,
  NT_CONDITIONALSTMT,
  NT_ELSEPART,
  NT_IOSTMT,
  NT_ARITHMETICEXPRESSION,
  NT_EXPPRIME,
  NT_TERM,
  NT_TERMPRIME,
  NT_FACTOR,
  NT_HIGHPRECEDENCEOPERATORS,
  NT_LOWPRECEDENCEOPERATORS,
  NT_BOOLEANEXPRESSION,
  NT_VAR,
  NT_LOGICALOP,
  NT_RELATIONALOP,
  NT_RETURNSTMT,
  NT_OPTIONALRETURN,
  NT_IDLIST,
  NT_MORE_IDS,
  NT_DEFINETYPESTMT,
  NT_A,
} NON_TERMINAL;

typedef struct {
  bool isTerminalSymbol;
  union {
    TOKEN_TYPE terminalType;
    NON_TERMINAL nonTerminalType;
  } symbolData;
} GrammarSymbol;

typedef struct {
  GrammarSymbol symbolList[PRODUCTION_MAX_LEN];
  int symbolCount;
} ProductionRule;

typedef struct {
  ProductionRule productionSet[NONTERMINAL_TOTAL][PRODUCTION_MAX_LEN];
  int productionCount[NONTERMINAL_TOTAL];
  bool derivesEpsilon[NONTERMINAL_TOTAL];
} GrammarSpec;

typedef struct {
  TOKEN_TYPE firstSet[NONTERMINAL_TOTAL][PRODUCTION_MAX_LEN];
  int firstSetSize[NONTERMINAL_TOTAL];
  bool firstContainsEpsilon[NONTERMINAL_TOTAL];
  int productionIndex[NONTERMINAL_TOTAL][PRODUCTION_MAX_LEN];

  TOKEN_TYPE followSet[NONTERMINAL_TOTAL][PRODUCTION_MAX_LEN];
  int followSetSize[NONTERMINAL_TOTAL];
  int epsilonProductionIdx[NONTERMINAL_TOTAL];
} FirstFollowSets;

typedef struct {
  int entries[NONTERMINAL_TOTAL][NUM_TOKEN_TYPES];
} ParsingTable;

typedef struct {
  GrammarSymbol nodeSymbol;
  int lineNum;
  char *tokenText;
  int tokenTextLen;
} SyntaxTreeNode;

typedef struct SyntaxTree SyntaxTree;

struct SyntaxTree {
  SyntaxTreeNode nodeData;
  struct SyntaxTree *parentNode;
  int childCount;
  struct SyntaxTree *childNodes[PRODUCTION_MAX_LEN];
};

#endif
