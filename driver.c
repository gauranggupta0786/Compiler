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
#include "parser.h"
#include "parserDef.h"
#include "utils.h"
#include <time.h>

/* Forward declarations for handler functions */
static int displayMenuAndGetChoice(void);
static void handleRemoveComments(char *inputPath);
static void handlePrintTokens(char *inputPath);
static int handleParseAndPrint(char *inputPath, char *outputPath, GrammarSpec grammarDef, FirstFollowSets ffSets, ParsingTable parseTable);
static int handleParseWithTiming(char *inputPath, GrammarSpec grammarDef, FirstFollowSets ffSets, ParsingTable parseTable);

static const char *MAIN_MENU = "What do you want to do:\n\t0. Exit\n\t1. Remove "
                  "Comments\n\t2. Print Lexer Token List\n\t3. Parse Code and "
                  "Print Parse Tree\n\t4. Parse Code and Print Time Taken\n";

/* Main entry point - placed before helper functions */
int main(int argc, char *argv[]) {
  /* Validate command line arguments first */
  if (argc != 3) {
    printf("Invalid No. of Arguments required 2 got %d\n", argc - 1);
    return 1;
  }

  /* Initialize grammar and parsing structures */
  GrammarSpec grammarDef = loadGrammarRules();
  FirstFollowSets ffSets = deriveFirstFollowSets(grammarDef);
  ParsingTable parseTable;
  buildParseTable(&parseTable, ffSets);

  int isRunning = 1;
  
  while (isRunning) {
    int menuChoice = displayMenuAndGetChoice();

    if (menuChoice == 0) {
      isRunning = 0;
    } else if (menuChoice == 1) {
      handleRemoveComments(argv[1]);
    } else if (menuChoice == 2) {
      handlePrintTokens(argv[1]);
    } else if (menuChoice == 3) {
      int result = handleParseAndPrint(argv[1], argv[2], grammarDef, ffSets, parseTable);
      if (result != 0) return result;
    } else if (menuChoice == 4) {
      int result = handleParseWithTiming(argv[1], grammarDef, ffSets, parseTable);
      if (result != 0) return result;
    } else {
      printf("Invalid Input - Exiting!\n");
      isRunning = 0;
    }
  }
  
  return 0;
}

/* Handler for option 4: Parse with timing */
static int handleParseWithTiming(char *inputPath, GrammarSpec grammarDef, FirstFollowSets ffSets, ParsingTable parseTable) {
  printf("==== Parsing... ====\n");
  FILE *codeFile = fopen(inputPath, "r");
  
  if (codeFile == NULL) {
    printf("Invalid Input File!\n");
    return 1;
  }
  
  clock_t timeStart = clock();
  SyntaxTree *parsedTree = performSyntaxAnalysis(codeFile, grammarDef, ffSets, parseTable);
  clock_t timeEnd = clock();
  
  printf("Source Code Parsed");
  printf("Time Taken to Parse: %ld\n", timeEnd - timeStart);
  printf("Time Taken to Parse (seconds): %lf\n",
         (float)(timeEnd - timeStart) / CLOCKS_PER_SEC);
  fclose(codeFile);
  return 0;
}

/* Handler for option 3: Parse and print tree */
static int handleParseAndPrint(char *inputPath, char *outputPath, GrammarSpec grammarDef, FirstFollowSets ffSets, ParsingTable parseTable) {
  printf("==== Parsing... ====\n");
  FILE *sourceInput = fopen(inputPath, "r");
  FILE *outputDest = fopen(outputPath, "w");
  
  if (sourceInput == NULL) {
    printf("Invalid Input File!\n");
    return 1;
  }

  SyntaxTree *treeRoot = performSyntaxAnalysis(sourceInput, grammarDef, ffSets, parseTable);
  outputParseTree(outputDest, treeRoot);
  printf("Source Code Parsed and parse tree printed into %s\n", outputPath);
  fclose(sourceInput);
  fclose(outputDest);
  return 0;
}

/* Handler for option 2: Print lexer tokens */
static void handlePrintTokens(char *inputPath) {
  printf("==== Lexer Tokens ====\n");
  FILE *srcFile = fopen(inputPath, "r");
  initializeInputStream(srcFile);
  printf("==== Lexer Tokens Complete ====\n\n\n");
}

/* Handler for option 1: Remove comments */
static void handleRemoveComments(char *inputPath) {
  printf("==== Comment Free Code ====\n");
  stripComments(inputPath);
  printf("==== Comment Removal Complete ====\n\n\n");
}

/* Helper function to display menu and get user choice */
static int displayMenuAndGetChoice(void) {
  int userChoice;
  printf("%s", MAIN_MENU);
  printf("==> ");
  scanf("%d", &userChoice);
  return userChoice;
}
