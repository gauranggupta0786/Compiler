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


#include "utils.h"
#include "lexerDef.h"
#include "parserDef.h"
#include <stdlib.h>

/* Check if token already exists in first set */
static bool isTokenInFirstSet(FirstFollowSets *ffSets, NON_TERMINAL nt, TOKEN_TYPE tok, int setSize) {
  int idx = 0;
  while (idx < setSize) {
    if (ffSets->firstSet[nt][idx] == tok) {
      return true;
    }
    idx++;
  }
  return false;
}

void mergeFirstSets(NON_TERMINAL nt1, NON_TERMINAL nt2, FirstFollowSets *ffSets) {
  int nt1NextIdx = ffSets->firstSetSize[nt1];
  int idx = 0;
  
  while (idx < ffSets->firstSetSize[nt2]) {
    TOKEN_TYPE tok = ffSets->firstSet[nt2][idx];
    
    if (!isTokenInFirstSet(ffSets, nt1, tok, nt1NextIdx)) {
      ffSets->firstSet[nt1][nt1NextIdx] = tok;
      nt1NextIdx++;
    }
    idx++;
  }
  
  ffSets->firstSetSize[nt1] = nt1NextIdx;
}

void computeFirstRecursive(GrammarSpec grammarDef, bool *completionFlags, NON_TERMINAL ntSymbol, FirstFollowSets *ffSets) {
  /* Skip if already computed */
  if (completionFlags[ntSymbol]) {
    return;
  }

  /* Process each production rule for this non-terminal */
  int ruleIdx = 0;
  while (ruleIdx < grammarDef.productionCount[ntSymbol]) {
    ProductionRule production = grammarDef.productionSet[ntSymbol][ruleIdx];
    int prevCount = ffSets->firstSetSize[ntSymbol];
    int elemIdx = 0;

    /* Iterate through elements of the rule */
    while (elemIdx < production.symbolCount) {
      GrammarSymbol currentElem = production.symbolList[elemIdx];

      /* Terminal found - add to first set and stop */
      if (currentElem.isTerminalSymbol) {
        TOKEN_TYPE termType = currentElem.symbolData.terminalType;
        ffSets->firstSet[ntSymbol][(ffSets->firstSetSize[ntSymbol])++] = termType;
        break;
      }

      /* Process non-terminal element */
      NON_TERMINAL currNt = currentElem.symbolData.nonTerminalType;
      computeFirstRecursive(grammarDef, completionFlags, currNt, ffSets);
      mergeFirstSets(ntSymbol, currNt, ffSets);

      /* Stop if current NT doesn't derive epsilon */
      if (!ffSets->firstContainsEpsilon[currNt]) {
        break;
      }
      
      /* Mark epsilon production if at last element */
      if (elemIdx == production.symbolCount - 1) {
        ffSets->epsilonProductionIdx[ntSymbol] = ruleIdx;
      }
      elemIdx++;
    }

    /* Update production indices for new first set entries */
    int addIdx = prevCount;
    while (addIdx < ffSets->firstSetSize[ntSymbol]) {
      ffSets->productionIndex[ntSymbol][addIdx] = ruleIdx;
      addIdx++;
    }
    
    ruleIdx++;
  }

  /* Handle epsilon productions */
  if (grammarDef.derivesEpsilon[ntSymbol] && !ffSets->firstContainsEpsilon[ntSymbol]) {
    ffSets->firstContainsEpsilon[ntSymbol] = true;
    ffSets->epsilonProductionIdx[ntSymbol] = grammarDef.productionCount[ntSymbol];
  }

  /* Mark non-terminal as computed */
  completionFlags[ntSymbol] = true;
}

/* Check if token exists in follow set */
static bool isTokenInFollowSet(FirstFollowSets *ffSets, NON_TERMINAL ntSymbol, TOKEN_TYPE tokenType) {
  int idx = 0;
  while (idx < ffSets->followSetSize[ntSymbol]) {
    if (ffSets->followSet[ntSymbol][idx] == tokenType) {
      return true;
    }
    idx++;
  }
  return false;
}

void appendToFollowSet(TOKEN_TYPE tokenType, NON_TERMINAL ntSymbol, FirstFollowSets *ffSets) {
  /* Add token to follow set if not already present */
  if (isTokenInFollowSet(ffSets, ntSymbol, tokenType)) {
    return;
  }
  ffSets->followSet[ntSymbol][(ffSets->followSetSize[ntSymbol])++] = tokenType;
}

/* Check if dependency already exists */
static bool hasDependency(NON_TERMINAL ntSymbol, NON_TERMINAL depNt, NON_TERMINAL **followDeps, int *depCounts) {
  int idx = 0;
  while (idx < depCounts[ntSymbol]) {
    if (followDeps[ntSymbol][idx] == depNt) {
      return true;
    }
    idx++;
  }
  return false;
}

void appendFollowDependency(int *depCounts, NON_TERMINAL **followDeps, NON_TERMINAL depNt, NON_TERMINAL ntSymbol) {
  if (hasDependency(ntSymbol, depNt, followDeps, depCounts)) {
    return;
  }
  followDeps[ntSymbol][(depCounts[ntSymbol])++] = depNt;
}

void processFollowSet(NON_TERMINAL **followDeps, int *depCounts, NON_TERMINAL lhsSymbol, ProductionRule production, FirstFollowSets *ffSets) {
  int elemIdx = 0;
  
  while (elemIdx < production.symbolCount - 1) {
    /* Skip terminal symbols */
    if (production.symbolList[elemIdx].isTerminalSymbol) {
      elemIdx++;
      continue;
    }

    NON_TERMINAL ntSymbol = production.symbolList[elemIdx].symbolData.nonTerminalType;
    int nextIdx = elemIdx + 1;
    
    /* Process subsequent symbols */
    while (nextIdx < production.symbolCount) {
      GrammarSymbol nextSym = production.symbolList[nextIdx];
      
      if (nextSym.isTerminalSymbol) {
        appendToFollowSet(nextSym.symbolData.terminalType, ntSymbol, ffSets);
        break;
      }

      NON_TERMINAL currNt = nextSym.symbolData.nonTerminalType;
      int firstIdx = 0;
      
      while (firstIdx < ffSets->firstSetSize[currNt]) {
        appendToFollowSet(ffSets->firstSet[currNt][firstIdx], ntSymbol, ffSets);
        firstIdx++;
      }
      
      if (!ffSets->firstContainsEpsilon[currNt]) {
        break;
      }
      nextIdx++;
    }

    if (nextIdx == production.symbolCount) {
      appendFollowDependency(depCounts, followDeps, lhsSymbol, ntSymbol);
    }
    
    elemIdx++;
  }

  /* Handle last element if non-terminal */
  GrammarSymbol lastElem = production.symbolList[production.symbolCount - 1];
  if (!lastElem.isTerminalSymbol) {
    appendFollowDependency(depCounts, followDeps, lhsSymbol, lastElem.symbolData.nonTerminalType);
  }
}

void resolveFollowDeps(FirstFollowSets *ffSets, int *depCounts, NON_TERMINAL **followDeps, NON_TERMINAL ntSymbol) {
  /* Base case: no dependencies to clear */
  if (depCounts[ntSymbol] == 0) {
    return;
  }
  
  int idx = 0;
  while (depCounts[ntSymbol] > 0) {
    NON_TERMINAL depNt = followDeps[ntSymbol][idx];
    depCounts[ntSymbol]--;
    idx++;
    
    /* Recursively clear dependencies */
    resolveFollowDeps(ffSets, depCounts, followDeps, depNt);

    /* Add follow set of dependent NT to current NT's follow set */
    int followIdx = 0;
    while (followIdx < ffSets->followSetSize[depNt]) {
      appendToFollowSet(ffSets->followSet[depNt][followIdx], ntSymbol, ffSets);
      followIdx++;
    }
  }
}

char *nonTerminalToString(NON_TERMINAL ntSymbol) {
  switch (ntSymbol) {
  case NT_PROGRAM:
    return "NT_PROGRAM";
  case NT_MAINFUNCTION:
    return "NT_MAINFUNCTION";
  case NT_OTHERFUNCTIONS:
    return "NT_OTHERFUNCTIONS";
  case NT_FUNCTION:
    return "NT_FUNCTION";
  case NT_INPUT_PAR:
    return "NT_INPUT_PAR";
  case NT_OUTPUT_PAR:
    return "NT_OUTPUT_PAR";
  case NT_PARAMETER_LIST:
    return "NT_PARAMETER_LIST";
  case NT_DATATYPE:
    return "NT_DATATYPE";
  case NT_PRIMITIVEDATATYPE:
    return "NT_PRIMITIVEDATATYPE";
  case NT_CONSTRUCTEDDATATYPE:
    return "NT_CONSTRUCTEDDATATYPE";
  case NT_REMAINING_LIST:
    return "NT_REMAINING_LIST";
  case NT_STMTS:
    return "NT_STMTS";
  case NT_TYPEDEFINITIONS:
    return "NT_TYPEDEFINITIONS";
  case NT_ACTUALORREDEFINED:
    return "NT_ACTUALORREDEFINED";
  case NT_TYPEDEFINITION:
    return "NT_TYPEDEFINITION";
  case NT_FIELDDEFINITIONS:
    return "NT_FIELDDEFINITIONS";
  case NT_FIELDDEFINITION:
    return "NT_FIELDDEFINITION";
  case NT_FIELDTYPE:
    return "NT_FIELDTYPE";
  case NT_MOREFIELDS:
    return "NT_MOREFIELDS";
  case NT_DECLATRATIONS:
    return "NT_DECLATRATIONS";
  case NT_DECLATRATION:
    return "NT_DECLATRATION";
  case NT_GLOBAL_OR_NOT:
    return "NT_GLOBAL_OR_NOT";
  case NT_OTHERSTMTS:
    return "NT_OTHERSTMTS";
  case NT_STMT:
    return "NT_STMT";
  case NT_ASSIGNMENTSTMT:
    return "NT_ASSIGNMENTSTMT";
  case NT_SINGLEORRECID:
    return "NT_SINGLEORRECID";
  case NT_OPTION_SINGLE_CONSTRUCTED:
    return "NT_OPTION_SINGLE_CONSTRUCTED";
  case NT_ONEEXPANSION:
    return "NT_ONEEXPANSION";
  case NT_MOREEXPANSIONS:
    return "NT_MOREEXPANSIONS";
  case NT_FUNCALLSTMT:
    return "NT_FUNCALLSTMT";
  case NT_OUTPUTPARAMETERS:
    return "NT_OUTPUTPARAMETERS";
  case NT_INPUTPARAMETERS:
    return "NT_INPUTPARAMETERS";
  case NT_ITERATIVESTMT:
    return "NT_ITERATIVESTMT";
  case NT_CONDITIONALSTMT:
    return "NT_CONDITIONALSTMT";
  case NT_ELSEPART:
    return "NT_ELSEPART";
  case NT_IOSTMT:
    return "NT_IOSTMT";
  case NT_ARITHMETICEXPRESSION:
    return "NT_ARITHMETICEXPRESSION";
  case NT_EXPPRIME:
    return "NT_EXPPRIME";
  case NT_TERM:
    return "NT_TERM";
  case NT_TERMPRIME:
    return "NT_TERMPRIME";
  case NT_FACTOR:
    return "NT_FACTOR";
  case NT_HIGHPRECEDENCEOPERATORS:
    return "NT_HIGHPRECEDENCEOPERATORS";
  case NT_LOWPRECEDENCEOPERATORS:
    return "NT_LOWPRECEDENCEOPERATORS";
  case NT_BOOLEANEXPRESSION:
    return "NT_BOOLEANEXPRESSION";
  case NT_VAR:
    return "NT_VAR";
  case NT_LOGICALOP:
    return "NT_LOGICALOP";
  case NT_RELATIONALOP:
    return "NT_RELATIONALOP";
  case NT_RETURNSTMT:
    return "NT_RETURNSTMT";
  case NT_OPTIONALRETURN:
    return "NT_OPTIONALRETURN";
  case NT_IDLIST:
    return "NT_IDLIST";
  case NT_MORE_IDS:
    return "NT_MORE_IDS";
  case NT_DEFINETYPESTMT:
    return "NT_DEFINETYPESTMT";
  case NT_A:
    return "NT_A";
  default:
    return "INVALID_NON_TERMINAL";
  }
}

GrammarSpec loadGrammarRules() {
  GrammarSpec grammarDef;

  // <program> -> <otherFunctions> <mainFunction>
  grammarDef.productionSet[NT_PROGRAM][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_OTHERFUNCTIONS}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_MAINFUNCTION}}},
      .symbolCount = 2};
  grammarDef.productionCount[NT_PROGRAM] = 1;
  grammarDef.derivesEpsilon[NT_PROGRAM] = false;

  // <mainFunction> -> TK_MAIN <stmts> TK_END
  grammarDef.productionSet[NT_MAINFUNCTION][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_MAIN}},
                                  (GrammarSymbol){false, {.nonTerminalType = NT_STMTS}},
                                  (GrammarSymbol){true, {.terminalType = TK_END}}},
                     .symbolCount = 3};
  grammarDef.productionCount[NT_MAINFUNCTION] = 1;
  grammarDef.derivesEpsilon[NT_MAINFUNCTION] = false;

  // <otherFunctions> -> <function><otherFunctions> | ε
  grammarDef.productionSet[NT_OTHERFUNCTIONS][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_FUNCTION}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_OTHERFUNCTIONS}}},
      .symbolCount = 2};
  grammarDef.productionSet[NT_OTHERFUNCTIONS][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_OTHERFUNCTIONS] = 1;
  grammarDef.derivesEpsilon[NT_OTHERFUNCTIONS] = true; // this handles ε

  // <function> -> TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END
  grammarDef.productionSet[NT_FUNCTION][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_FUNID}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_INPUT_PAR}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_OUTPUT_PAR}},
                   (GrammarSymbol){true, {.terminalType = TK_SEM}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_STMTS}},
                   (GrammarSymbol){true, {.terminalType = TK_END}}},
      .symbolCount = 6};
  grammarDef.productionCount[NT_FUNCTION] = 1;
  grammarDef.derivesEpsilon[NT_FUNCTION] = false;

  // <input_par> -> TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list>
  // TK_SQR
  grammarDef.productionSet[NT_INPUT_PAR][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_INPUT}},
                   (GrammarSymbol){true, {.terminalType = TK_PARAMETER}},
                   (GrammarSymbol){true, {.terminalType = TK_LIST}},
                   (GrammarSymbol){true, {.terminalType = TK_SQL}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_PARAMETER_LIST}},
                   (GrammarSymbol){true, {.terminalType = TK_SQR}}},
      .symbolCount = 6};
  grammarDef.productionCount[NT_INPUT_PAR] = 1;
  grammarDef.derivesEpsilon[NT_INPUT_PAR] = false;

  // <output_par> -> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <<parameter_list>
  // TK_SQR | ε
  grammarDef.productionSet[NT_OUTPUT_PAR][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_OUTPUT}},
                   (GrammarSymbol){true, {.terminalType = TK_PARAMETER}},
                   (GrammarSymbol){true, {.terminalType = TK_LIST}},
                   (GrammarSymbol){true, {.terminalType = TK_SQL}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_PARAMETER_LIST}},
                   (GrammarSymbol){true, {.terminalType = TK_SQR}}},
      .symbolCount = 6};
  grammarDef.productionSet[NT_OUTPUT_PAR][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_OUTPUT_PAR] = 1;
  grammarDef.derivesEpsilon[NT_OUTPUT_PAR] = true;

  // <parameter_list> -> <dataType> TK_ID <remaining_list>
  grammarDef.productionSet[NT_PARAMETER_LIST][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_DATATYPE}},
                   (GrammarSymbol){true, {.terminalType = TK_ID}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_REMAINING_LIST}}},
      .symbolCount = 3};
  grammarDef.productionCount[NT_PARAMETER_LIST] = 1;
  grammarDef.derivesEpsilon[NT_PARAMETER_LIST] = false;

  // <dataType> -> <primitiveDatatype> | <constructedDatatype>
  grammarDef.productionSet[NT_DATATYPE][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_PRIMITIVEDATATYPE}}},
      .symbolCount = 1};
  grammarDef.productionSet[NT_DATATYPE][1] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_CONSTRUCTEDDATATYPE}}},
      .symbolCount = 1};
  grammarDef.productionCount[NT_DATATYPE] = 2;
  grammarDef.derivesEpsilon[NT_DATATYPE] = false;

  // <primitiveDatatype> -> TK_INT | TK_REAL
  grammarDef.productionSet[NT_PRIMITIVEDATATYPE][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_INT}}}, .symbolCount = 1};
  grammarDef.productionSet[NT_PRIMITIVEDATATYPE][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_REAL}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_PRIMITIVEDATATYPE] = 2;
  grammarDef.derivesEpsilon[NT_PRIMITIVEDATATYPE] = false;

  // <constructedDatatype> -> TK_RECORD TK_RUID | TK_UNION TK_RUID | TK_RUID
  grammarDef.productionSet[NT_CONSTRUCTEDDATATYPE][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_RECORD}},
                                  (GrammarSymbol){true, {.terminalType = TK_RUID}}},
                     .symbolCount = 2};
  grammarDef.productionSet[NT_CONSTRUCTEDDATATYPE][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_UNION}},
                                  (GrammarSymbol){true, {.terminalType = TK_RUID}}},
                     .symbolCount = 2};
  grammarDef.productionSet[NT_CONSTRUCTEDDATATYPE][2] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_RUID}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_CONSTRUCTEDDATATYPE] = 3;
  grammarDef.derivesEpsilon[NT_CONSTRUCTEDDATATYPE] = false;

  // <remaining_list> -> TK_COMMA <parameter_list> | ε
  grammarDef.productionSet[NT_REMAINING_LIST][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_COMMA}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_PARAMETER_LIST}}},
      .symbolCount = 2};
  grammarDef.productionSet[NT_REMAINING_LIST][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_REMAINING_LIST] = 1;
  grammarDef.derivesEpsilon[NT_REMAINING_LIST] = true;

  // <stmts> -> <typeDefinitions> <declatrations> <otherStmts> <returnStmt>
  grammarDef.productionSet[NT_STMTS][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_TYPEDEFINITIONS}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_DECLATRATIONS}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_OTHERSTMTS}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_RETURNSTMT}}},
      .symbolCount = 4};
  grammarDef.productionCount[NT_STMTS] = 1;
  grammarDef.derivesEpsilon[NT_STMTS] = false;

  // <typeDefinitions> -> <actualOrRedefined> <typeDefinitions> | ε
  grammarDef.productionSet[NT_TYPEDEFINITIONS][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_ACTUALORREDEFINED}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_TYPEDEFINITIONS}}},
      .symbolCount = 2};
  grammarDef.productionSet[NT_TYPEDEFINITIONS][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_TYPEDEFINITIONS] = 1;
  grammarDef.derivesEpsilon[NT_TYPEDEFINITIONS] = true;

  // <actualOrRedefined> -> <typeDefinition> | <definetypestmt>
  grammarDef.productionSet[NT_ACTUALORREDEFINED][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_TYPEDEFINITION}}},
      .symbolCount = 1};
  grammarDef.productionSet[NT_ACTUALORREDEFINED][1] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_DEFINETYPESTMT}}},
      .symbolCount = 1};
  grammarDef.productionCount[NT_ACTUALORREDEFINED] = 2;
  grammarDef.derivesEpsilon[NT_ACTUALORREDEFINED] = false;

  // <typeDefinition> -> TK_RECORD TK_RUID <fieldDefinitions> TK_ENDRECORD
  // <typeDefinition> -> TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION
  grammarDef.productionSet[NT_TYPEDEFINITION][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_RECORD}},
                   (GrammarSymbol){true, {.terminalType = TK_RUID}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_FIELDDEFINITIONS}},
                   (GrammarSymbol){true, {.terminalType = TK_ENDRECORD}}},
      .symbolCount = 4};
  grammarDef.productionSet[NT_TYPEDEFINITION][1] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_UNION}},
                   (GrammarSymbol){true, {.terminalType = TK_RUID}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_FIELDDEFINITIONS}},
                   (GrammarSymbol){true, {.terminalType = TK_ENDUNION}}},
      .symbolCount = 4};
  grammarDef.productionCount[NT_TYPEDEFINITION] = 2;
  grammarDef.derivesEpsilon[NT_TYPEDEFINITION] = false;

  // <fieldDefinitions> -> <fieldDefinition> <fieldDefinition> <moreFields>
  grammarDef.productionSet[NT_FIELDDEFINITIONS][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_FIELDDEFINITION}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_FIELDDEFINITION}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_MOREFIELDS}}},
      .symbolCount = 3};
  grammarDef.productionCount[NT_FIELDDEFINITIONS] = 1;
  grammarDef.derivesEpsilon[NT_FIELDDEFINITIONS] = false;

  // <fieldDefinition> -> TK_TYPE <fieldType> TK_COLON TK_FIELDID TK_SEM
  grammarDef.productionSet[NT_FIELDDEFINITION][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_TYPE}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_FIELDTYPE}},
                   (GrammarSymbol){true, {.terminalType = TK_COLON}},
                   (GrammarSymbol){true, {.terminalType = TK_FIELDID}},
                   (GrammarSymbol){true, {.terminalType = TK_SEM}}},
      .symbolCount = 5};
  grammarDef.productionCount[NT_FIELDDEFINITION] = 1;
  grammarDef.derivesEpsilon[NT_FIELDDEFINITION] = false;

  // <fieldType> -> <primitiveDatatype> | <constructedDatatype>
  grammarDef.productionSet[NT_FIELDTYPE][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_PRIMITIVEDATATYPE}}},
      .symbolCount = 1};
  grammarDef.productionSet[NT_FIELDTYPE][1] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_CONSTRUCTEDDATATYPE}}},
      .symbolCount = 1};
  grammarDef.productionCount[NT_FIELDTYPE] = 2;
  grammarDef.derivesEpsilon[NT_FIELDTYPE] = false;

  // <moreFields> -> <fieldDefinition> <moreFields> | ε
  grammarDef.productionSet[NT_MOREFIELDS][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_FIELDDEFINITION}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_MOREFIELDS}}},
      .symbolCount = 2};
  grammarDef.productionSet[NT_MOREFIELDS][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_MOREFIELDS] = 1;
  grammarDef.derivesEpsilon[NT_MOREFIELDS] = true;

  // <declatrations> -> <declatration> <declatrations> | ε
  grammarDef.productionSet[NT_DECLATRATIONS][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_DECLATRATION}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_DECLATRATIONS}}},
      .symbolCount = 2};
  grammarDef.productionSet[NT_DECLATRATIONS][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_DECLATRATIONS] = 1;
  grammarDef.derivesEpsilon[NT_DECLATRATIONS] = true;

  // <declatration> -> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not>
  // TK_SEM
  grammarDef.productionSet[NT_DECLATRATION][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_TYPE}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_DATATYPE}},
                   (GrammarSymbol){true, {.terminalType = TK_COLON}},
                   (GrammarSymbol){true, {.terminalType = TK_ID}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_GLOBAL_OR_NOT}},
                   (GrammarSymbol){true, {.terminalType = TK_SEM}}},
      .symbolCount = 6};
  grammarDef.productionCount[NT_DECLATRATION] = 1;
  grammarDef.derivesEpsilon[NT_DECLATRATION] = false;

  // <global_or_not> -> TK_COLON TK_GLOBAL | ε
  grammarDef.productionSet[NT_GLOBAL_OR_NOT][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_COLON}},
                                  (GrammarSymbol){true, {.terminalType = TK_GLOBAL}}},
                     .symbolCount = 2};
  grammarDef.productionSet[NT_GLOBAL_OR_NOT][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_GLOBAL_OR_NOT] = 1;
  grammarDef.derivesEpsilon[NT_GLOBAL_OR_NOT] = true;

  //<otherStmts> -> <stmt> <otherStmts> | ε
  grammarDef.productionSet[NT_OTHERSTMTS][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_STMT}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_OTHERSTMTS}}},
      .symbolCount = 2};
  grammarDef.productionSet[NT_OTHERSTMTS][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_OTHERSTMTS] = 1;
  grammarDef.derivesEpsilon[NT_OTHERSTMTS] = true;

  // <stmt> -> <assignmentStmt> | <iterativeStmt> | <conditionalStmt> |
  // <ioStmt> | <funCallStmt>
  grammarDef.productionSet[NT_STMT][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_ASSIGNMENTSTMT}}},
      .symbolCount = 1};
  grammarDef.productionSet[NT_STMT][1] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_ITERATIVESTMT}}},
      .symbolCount = 1};
  grammarDef.productionSet[NT_STMT][2] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_CONDITIONALSTMT}}},
      .symbolCount = 1};
  grammarDef.productionSet[NT_STMT][3] =
      (ProductionRule){.symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_IOSTMT}}},
                     .symbolCount = 1};
  grammarDef.productionSet[NT_STMT][4] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_FUNCALLSTMT}}},
      .symbolCount = 1};
  grammarDef.productionCount[NT_STMT] = 5;
  grammarDef.derivesEpsilon[NT_STMT] = false;

  // <assignmentStmt> -> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression>
  // TK_SEM
  grammarDef.productionSet[NT_ASSIGNMENTSTMT][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_SINGLEORRECID}},
                   (GrammarSymbol){true, {.terminalType = TK_ASSIGNOP}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_ARITHMETICEXPRESSION}},
                   (GrammarSymbol){true, {.terminalType = TK_SEM}}},
      .symbolCount = 4};
  grammarDef.productionCount[NT_ASSIGNMENTSTMT] = 1;
  grammarDef.derivesEpsilon[NT_ASSIGNMENTSTMT] = false;

  // <singleOrRecId> -> TK_ID <option_single_constructed>
  grammarDef.productionSet[NT_SINGLEORRECID][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_ID}},
                   (GrammarSymbol){false,
                                     {.nonTerminalType = NT_OPTION_SINGLE_CONSTRUCTED}}},
      .symbolCount = 2};
  grammarDef.productionCount[NT_SINGLEORRECID] = 1;
  grammarDef.derivesEpsilon[NT_SINGLEORRECID] = false;

  // <option_single_constructed> -> <oneExpansion> <moreExpansions> | ε
  grammarDef.productionSet[NT_OPTION_SINGLE_CONSTRUCTED][0] = (ProductionRule){
      .symbolList =
          {
              (GrammarSymbol){false, {.nonTerminalType = NT_ONEEXPANSION}},
              (GrammarSymbol){false, {.nonTerminalType = NT_MOREEXPANSIONS}},
          },
      .symbolCount = 2};
  grammarDef.productionSet[NT_OPTION_SINGLE_CONSTRUCTED][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_OPTION_SINGLE_CONSTRUCTED] = 1;
  grammarDef.derivesEpsilon[NT_OPTION_SINGLE_CONSTRUCTED] = true;

  // <oneExpansion> -> TK_DOT TK_FIELDID
  grammarDef.productionSet[NT_ONEEXPANSION][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_DOT}},
                                  (GrammarSymbol){true, {.terminalType = TK_FIELDID}}},
                     .symbolCount = 2};
  grammarDef.productionCount[NT_ONEEXPANSION] = 1;
  grammarDef.derivesEpsilon[NT_ONEEXPANSION] = false;

  // <moreExpansions> -> <oneExpansion> <moreExpansions> | ε
  grammarDef.productionSet[NT_MOREEXPANSIONS][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_ONEEXPANSION}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_MOREEXPANSIONS}}},
      .symbolCount = 2};
  grammarDef.productionSet[NT_MOREEXPANSIONS][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_MOREEXPANSIONS] = 1;
  grammarDef.derivesEpsilon[NT_MOREEXPANSIONS] = true;

  // <funCallStmt> -> <outputParameters> TK_CALL TK_FUNID TK_WITH
  // TK_PARAMETERS <inputParameters> TK_SEM
  grammarDef.productionSet[NT_FUNCALLSTMT][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_OUTPUTPARAMETERS}},
                   (GrammarSymbol){true, {.terminalType = TK_CALL}},
                   (GrammarSymbol){true, {.terminalType = TK_FUNID}},
                   (GrammarSymbol){true, {.terminalType = TK_WITH}},
                   (GrammarSymbol){true, {.terminalType = TK_PARAMETERS}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_INPUTPARAMETERS}},
                   (GrammarSymbol){true, {.terminalType = TK_SEM}}},
      .symbolCount = 7};
  grammarDef.productionCount[NT_FUNCALLSTMT] = 1;
  grammarDef.derivesEpsilon[NT_FUNCALLSTMT] = false;

  // <outputParameters> -> TK_SQL <idList> TK_SQR TK_ASSIGNOP | ε
  grammarDef.productionSet[NT_OUTPUTPARAMETERS][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_SQL}},
                                  (GrammarSymbol){false, {.nonTerminalType = NT_IDLIST}},
                                  (GrammarSymbol){true, {.terminalType = TK_SQR}},
                                  (GrammarSymbol){true, {.terminalType = TK_ASSIGNOP}}},
                     .symbolCount = 4};
  grammarDef.productionSet[NT_OUTPUTPARAMETERS][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_OUTPUTPARAMETERS] = 1;
  grammarDef.derivesEpsilon[NT_OUTPUTPARAMETERS] = true;

  // <inputParameters> -> TK_SQL <idList> TK_SQR
  grammarDef.productionSet[NT_INPUTPARAMETERS][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_SQL}},
                                  (GrammarSymbol){false, {.nonTerminalType = NT_IDLIST}},
                                  (GrammarSymbol){true, {.terminalType = TK_SQR}}},
                     .symbolCount = 3};
  grammarDef.productionCount[NT_INPUTPARAMETERS] = 1;
  grammarDef.derivesEpsilon[NT_INPUTPARAMETERS] = false;

  // <iterativeStmt> -> TK_WHILE TK_OP <booleanExpression> TK_CL <stmt>
  // <otherStmts> TK_ENDWHILE
  grammarDef.productionSet[NT_ITERATIVESTMT][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_WHILE}},
                   (GrammarSymbol){true, {.terminalType = TK_OP}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_BOOLEANEXPRESSION}},
                   (GrammarSymbol){true, {.terminalType = TK_CL}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_STMT}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_OTHERSTMTS}},
                   (GrammarSymbol){true, {.terminalType = TK_ENDWHILE}}},
      .symbolCount = 7};
  grammarDef.productionCount[NT_ITERATIVESTMT] = 1;
  grammarDef.derivesEpsilon[NT_ITERATIVESTMT] = false;

  // <conditionalStmt> -> TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt>
  // <otherStmts> <elsePart>
  grammarDef.productionSet[NT_CONDITIONALSTMT][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_IF}},
                   (GrammarSymbol){true, {.terminalType = TK_OP}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_BOOLEANEXPRESSION}},
                   (GrammarSymbol){true, {.terminalType = TK_CL}},
                   (GrammarSymbol){true, {.terminalType = TK_THEN}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_STMT}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_OTHERSTMTS}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_ELSEPART}}},
      .symbolCount = 8};
  grammarDef.productionCount[NT_CONDITIONALSTMT] = 1;
  grammarDef.derivesEpsilon[NT_CONDITIONALSTMT] = false;

  // <elsePart> -> TK_ELSE <stmt> <otherStmts> TK_ENDIF | TK_ENDIF
  grammarDef.productionSet[NT_ELSEPART][0] =
      (ProductionRule){.symbolList =
                         {
                             (GrammarSymbol){true, {.terminalType = TK_ELSE}},
                             (GrammarSymbol){false, {.nonTerminalType = NT_STMT}},
                             (GrammarSymbol){false, {.nonTerminalType = NT_OTHERSTMTS}},
                             (GrammarSymbol){true, {.terminalType = TK_ENDIF}},
                         },
                     .symbolCount = 4};
  grammarDef.productionSet[NT_ELSEPART][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_ENDIF}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_ELSEPART] = 2;
  grammarDef.derivesEpsilon[NT_ELSEPART] = false;

  // <ioStmt> -> TK_READ TK_OP <var> TK_CL TK_SEM | TK_WRITE TK_OP <var> TK_CL
  // TK_SEM
  grammarDef.productionSet[NT_IOSTMT][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_READ}},
                                  (GrammarSymbol){true, {.terminalType = TK_OP}},
                                  (GrammarSymbol){false, {.nonTerminalType = NT_VAR}},
                                  (GrammarSymbol){true, {.terminalType = TK_CL}},
                                  (GrammarSymbol){true, {.terminalType = TK_SEM}}},
                     .symbolCount = 5};
  grammarDef.productionSet[NT_IOSTMT][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_WRITE}},
                                  (GrammarSymbol){true, {.terminalType = TK_OP}},
                                  (GrammarSymbol){false, {.nonTerminalType = NT_VAR}},
                                  (GrammarSymbol){true, {.terminalType = TK_CL}},
                                  (GrammarSymbol){true, {.terminalType = TK_SEM}}},
                     .symbolCount = 5};
  grammarDef.productionCount[NT_IOSTMT] = 2;
  grammarDef.derivesEpsilon[NT_IOSTMT] = false;

  // <arithmeticExpression> -> <term> <expPrime>
  grammarDef.productionSet[NT_ARITHMETICEXPRESSION][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_TERM}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_EXPPRIME}}},
      .symbolCount = 2};
  grammarDef.productionCount[NT_ARITHMETICEXPRESSION] = 1;
  grammarDef.derivesEpsilon[NT_ARITHMETICEXPRESSION] = false;

  // <expPrime> -> <lowPrecedenceOperators> <term> <expPrime> | ε
  grammarDef.productionSet[NT_EXPPRIME][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_LOWPRECEDENCEOPERATORS}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_TERM}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_EXPPRIME}}},
      .symbolCount = 3};
  grammarDef.productionSet[NT_EXPPRIME][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_EXPPRIME] = 1;
  grammarDef.derivesEpsilon[NT_EXPPRIME] = true;

  // <term> -> <factor> <termPrime>
  grammarDef.productionSet[NT_TERM][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_FACTOR}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_TERMPRIME}}},
      .symbolCount = 2};
  grammarDef.productionCount[NT_TERM] = 1;
  grammarDef.derivesEpsilon[NT_TERM] = false;

  // <termPrime> -> <highPrecedenceOperators> <factor> <termPrime> | ε
  grammarDef.productionSet[NT_TERMPRIME][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_HIGHPRECEDENCEOPERATORS}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_FACTOR}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_TERMPRIME}}},
      .symbolCount = 3};
  grammarDef.productionSet[NT_TERMPRIME][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_TERMPRIME] = 1;
  grammarDef.derivesEpsilon[NT_TERMPRIME] = true;

  // <factor> -> TK_OP <arithmeticExpression> TK_CL | <var>
  grammarDef.productionSet[NT_FACTOR][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_OP}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_ARITHMETICEXPRESSION}},
                   (GrammarSymbol){true, {.terminalType = TK_CL}}},
      .symbolCount = 3};
  grammarDef.productionSet[NT_FACTOR][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_VAR}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_FACTOR] = 2;
  grammarDef.derivesEpsilon[NT_FACTOR] = false;

  // <highPrecedenceOperators> -> TK_MUL | TK_DIV
  grammarDef.productionSet[NT_HIGHPRECEDENCEOPERATORS][0] =
      (ProductionRule){.symbolList =
                         {
                             (GrammarSymbol){true, {.terminalType = TK_MUL}},
                         },
                     .symbolCount = 1};
  grammarDef.productionSet[NT_HIGHPRECEDENCEOPERATORS][1] =
      (ProductionRule){.symbolList =
                         {
                             (GrammarSymbol){true, {.terminalType = TK_DIV}},
                         },
                     .symbolCount = 1};
  grammarDef.productionCount[NT_HIGHPRECEDENCEOPERATORS] = 2;
  grammarDef.derivesEpsilon[NT_HIGHPRECEDENCEOPERATORS] = false;

  // <lowPrecedenceOperators> -> TK_PLUS | TK_MINUS
  grammarDef.productionSet[NT_LOWPRECEDENCEOPERATORS][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_PLUS}}},
                     .symbolCount = 1};
  grammarDef.productionSet[NT_LOWPRECEDENCEOPERATORS][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_MINUS}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_LOWPRECEDENCEOPERATORS] = 2;
  grammarDef.derivesEpsilon[NT_LOWPRECEDENCEOPERATORS] = false;

  // <booleanExpression> -> TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP
  // <booleanExpression> TK_CL
  // <booleanExpression> -> <var> <relationalOp> <var>
  // <booleanExpression> -> TK_NOT TK_OP <booleanExpression> TK_CL
  grammarDef.productionSet[NT_BOOLEANEXPRESSION][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_OP}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_BOOLEANEXPRESSION}},
                   (GrammarSymbol){true, {.terminalType = TK_CL}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_LOGICALOP}},
                   (GrammarSymbol){true, {.terminalType = TK_OP}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_BOOLEANEXPRESSION}},
                   (GrammarSymbol){true, {.terminalType = TK_CL}}},
      .symbolCount = 7};
  grammarDef.productionSet[NT_BOOLEANEXPRESSION][1] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_VAR}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_RELATIONALOP}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_VAR}}},
      .symbolCount = 3};
  grammarDef.productionSet[NT_BOOLEANEXPRESSION][2] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_NOT}},
                   (GrammarSymbol){true, {.terminalType = TK_OP}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_BOOLEANEXPRESSION}},
                   (GrammarSymbol){true, {.terminalType = TK_CL}}},
      .symbolCount = 4};
  grammarDef.productionCount[NT_BOOLEANEXPRESSION] = 3;
  grammarDef.derivesEpsilon[NT_BOOLEANEXPRESSION] = false;

  // <var> -> <singleOrRecId> | TK_NUM | TK_RNUM
  grammarDef.productionSet[NT_VAR][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){false, {.nonTerminalType = NT_SINGLEORRECID}}},
      .symbolCount = 1};
  grammarDef.productionSet[NT_VAR][1] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_NUM}}},
      .symbolCount = 1,
  };
  grammarDef.productionSet[NT_VAR][2] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_RNUM}}},
      .symbolCount = 1,
  };
  grammarDef.productionCount[NT_VAR] = 3;
  grammarDef.derivesEpsilon[NT_VAR] = false;

  // <logicalOp> -> TK_AND | TK_OR
  grammarDef.productionSet[NT_LOGICALOP][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_AND}}}, .symbolCount = 1};
  grammarDef.productionSet[NT_LOGICALOP][1] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_OR}}}, .symbolCount = 1};
  grammarDef.productionCount[NT_LOGICALOP] = 2;
  grammarDef.derivesEpsilon[NT_LOGICALOP] = false;

  // <relationalOp> -> TK_LT | TK_LE | TK_EQ | TK_GT | TK_GE | TK_NE
  grammarDef.productionSet[NT_RELATIONALOP][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_LT}}}, .symbolCount = 1};
  grammarDef.productionSet[NT_RELATIONALOP][1] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_LE}}}, .symbolCount = 1};
  grammarDef.productionSet[NT_RELATIONALOP][2] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_EQ}}}, .symbolCount = 1};
  grammarDef.productionSet[NT_RELATIONALOP][3] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_GT}}}, .symbolCount = 1};
  grammarDef.productionSet[NT_RELATIONALOP][4] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_GE}}}, .symbolCount = 1};
  grammarDef.productionSet[NT_RELATIONALOP][5] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_NE}}}, .symbolCount = 1};

  grammarDef.productionCount[NT_RELATIONALOP] = 6;
  grammarDef.derivesEpsilon[NT_RELATIONALOP] = false;

  // <returnStmt> -> TK_RETURN <optionalReturn> TK_SEM
  grammarDef.productionSet[NT_RETURNSTMT][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_RETURN}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_OPTIONALRETURN}},
                   (GrammarSymbol){true, {.terminalType = TK_SEM}}},
      .symbolCount = 3};
  grammarDef.productionCount[NT_RETURNSTMT] = 1;
  grammarDef.derivesEpsilon[NT_RETURNSTMT] = false;

  // <optionalReturn> -> TK_SQL <idList> TK_SQR | ε
  grammarDef.productionSet[NT_OPTIONALRETURN][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_SQL}},
                                  (GrammarSymbol){false, {.nonTerminalType = NT_IDLIST}},
                                  (GrammarSymbol){true, {.terminalType = TK_SQR}}},
                     .symbolCount = 3};
  grammarDef.productionSet[NT_OPTIONALRETURN][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_OPTIONALRETURN] = 1;
  grammarDef.derivesEpsilon[NT_OPTIONALRETURN] = true;

  // <idList> -> TK_ID <more_ids>
  grammarDef.productionSet[NT_IDLIST][0] = (ProductionRule){
      .symbolList = {(GrammarSymbol){true, {.terminalType = TK_ID}},
                   (GrammarSymbol){false, {.nonTerminalType = NT_MORE_IDS}}},
      .symbolCount = 2};
  grammarDef.productionCount[NT_IDLIST] = 1;
  grammarDef.derivesEpsilon[NT_IDLIST] = false;

  // <more_ids> -> TK_COMMA <idList> | ε
  grammarDef.productionSet[NT_MORE_IDS][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_COMMA}},
                                  (GrammarSymbol){false, {.nonTerminalType = NT_IDLIST}}},
                     .symbolCount = 2};
  grammarDef.productionSet[NT_MORE_IDS][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = EPSILLON}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_MORE_IDS] = 1;
  grammarDef.derivesEpsilon[NT_MORE_IDS] = true;

  // <definetypestmt> -> TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID
  grammarDef.productionSet[NT_DEFINETYPESTMT][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_DEFINETYPE}},
                                  (GrammarSymbol){false, {.nonTerminalType = NT_A}},
                                  (GrammarSymbol){true, {.terminalType = TK_RUID}},
                                  (GrammarSymbol){true, {.terminalType = TK_AS}},
                                  (GrammarSymbol){true, {.terminalType = TK_RUID}}},
                     .symbolCount = 5};
  grammarDef.productionCount[NT_DEFINETYPESTMT] = 1;
  grammarDef.derivesEpsilon[NT_DEFINETYPESTMT] = false;

  //<A> -> TK_RECORD | TK_UNION
  grammarDef.productionSet[NT_A][0] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_RECORD}}},
                     .symbolCount = 1};
  grammarDef.productionSet[NT_A][1] =
      (ProductionRule){.symbolList = {(GrammarSymbol){true, {.terminalType = TK_UNION}}},
                     .symbolCount = 1};
  grammarDef.productionCount[NT_A] = 2;
  grammarDef.derivesEpsilon[NT_A] = false;
  return grammarDef;
}
