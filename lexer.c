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
#include "string.h"
#include "trie.h"
#include <stdbool.h>
#include <stdlib.h>

static TrieNodePtr keywordTable;

// DFA state transition logic function
// INPUT: Present State, Read Symbol
// OUTPUT: {Next State, isReturningToken, Token Type, Redaction (if applicable), Error Type}
TransitionResult performDFATransition(DFA_STATE currState, char inputChar) {
  switch (currState) {
  case DFA_INITIAL: {
    if (inputChar == ';') {
      return (TransitionResult){DFA_INITIAL, true, TK_SEM, 0};
    } else if (inputChar == ',') {
      return (TransitionResult){DFA_INITIAL, true, TK_COMMA, 0};
    } else if (inputChar == '.') {
      return (TransitionResult){DFA_INITIAL, true, TK_DOT, 0};
    } else if (inputChar == '(') {
      return (TransitionResult){DFA_INITIAL, true, TK_OP, 0};
    } else if (inputChar == ')') {
      return (TransitionResult){DFA_INITIAL, true, TK_CL, 0};
    } else if (inputChar == '[') {
      return (TransitionResult){DFA_INITIAL, true, TK_SQL, 0};
    } else if (inputChar == ']') {
      return (TransitionResult){DFA_INITIAL, true, TK_SQR, 0};
    } else if (inputChar == '*') {
      return (TransitionResult){DFA_INITIAL, true, TK_MUL, 0};
    } else if (inputChar == '/') {
      return (TransitionResult){DFA_INITIAL, true, TK_DIV, 0};
    } else if (inputChar == '+') {
      return (TransitionResult){DFA_INITIAL, true, TK_PLUS, 0};
    } else if (inputChar == '-') {
      return (TransitionResult){DFA_INITIAL, true, TK_MINUS, 0};
    } else if (inputChar == '~') {
      return (TransitionResult){DFA_INITIAL, true, TK_NOT, 0};
    } else if (inputChar == '@') {
      return (TransitionResult){DFA_S13, false, NULL_TOKEN, 0};
    } else if (inputChar == '!') {
      return (TransitionResult){DFA_S16, false, NULL_TOKEN, 0};
    } else if (inputChar == '&') {
      return (TransitionResult){DFA_S18, false, NULL_TOKEN, 0};
    } else if (inputChar == '=') {
      return (TransitionResult){DFA_S21, false, NULL_TOKEN, 0};
    } else if (inputChar == '%') {
      return (TransitionResult){DFA_S23, false, NULL_TOKEN, 0};
    } else if (inputChar == EOF) {
      return (TransitionResult){DFA_INITIAL, true, EXIT_TOKEN, 0};
    } else if (inputChar == ':') {
      return (TransitionResult){DFA_INITIAL, true, TK_COLON, 0};
    } else if (inputChar == '<') {
      return (TransitionResult){DFA_S26, false, NULL_TOKEN, 0};
    } else if (inputChar == '>') {
      return (TransitionResult){DFA_S33, false, NULL_TOKEN, 0};
    } else if (inputChar == '_') {
      return (TransitionResult){DFA_S37, false, NULL_TOKEN, 0};
    } else if (inputChar == '#') {
      return (TransitionResult){DFA_S41, false, NULL_TOKEN, 0};
    } else if (inputChar >= '0' && inputChar <= '9') {
      return (TransitionResult){DFA_S44, false, NULL_TOKEN, 0};
    } else if (inputChar >= 'b' && inputChar <= 'd') {
      return (TransitionResult){DFA_S57, false, NULL_TOKEN, 0};
    } else if ((inputChar >= 'a' && inputChar <= 'z') ||
               (inputChar >= 'A' && inputChar <= 'Z')) {
      return (TransitionResult){DFA_S55, false, NULL_TOKEN, 0};
    } else if (inputChar == '\t' || inputChar == ' ') {
      return (TransitionResult){DFA_INITIAL, true, BLANK, 0};
    } else if (inputChar == '\n') {
      return (TransitionResult){DFA_INITIAL, true, NEWLINE, 0};
    } else if (inputChar == '\0') {
      return (TransitionResult){DFA_INITIAL, true, BLANK, 0};
    } else {
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0};
    }
  }
  case DFA_S13: {
    if (inputChar == '@')
      return (TransitionResult){DFA_S14, false, NULL_TOKEN, 0};
    else
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 1};
  }
  case DFA_S14: {
    if (inputChar == '@')
      return (TransitionResult){DFA_INITIAL, true, TK_OR, 0};
    else
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 1};
  }
  case DFA_S16: {
    if (inputChar == '=')
      return (TransitionResult){DFA_INITIAL, true, TK_NE, 0};
    else
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 2};
  }
  case DFA_S18: {
    if (inputChar == '&')
      return (TransitionResult){DFA_S19, false, NULL_TOKEN, 0};
    else
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 3};
  }
  case DFA_S19: {
    if (inputChar == '&')
      return (TransitionResult){DFA_INITIAL, true, TK_AND, 0};
    else
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 3};
  }
  case DFA_S21: {
    if (inputChar == '=')
      return (TransitionResult){DFA_INITIAL, true, TK_EQ, 0};
    else
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 4};
  }
  case DFA_S23: {
    if (inputChar != '\n' && inputChar != '\0')
      return (TransitionResult){DFA_S23, false, NULL_TOKEN, 0};
    else
      return (TransitionResult){DFA_INITIAL, true, TK_COMMENT, 0};
  }
  case DFA_S26: {
    if (inputChar == '-')
      return (TransitionResult){DFA_S28, false, NULL_TOKEN, 0};
    else if (inputChar == '=')
      return (TransitionResult){DFA_INITIAL, true, TK_LE, 0};
    else
      return (TransitionResult){DFA_INITIAL, true, TK_LT, 1};
  }
  case DFA_S28: {
    if (inputChar == '-')
      return (TransitionResult){DFA_S29, false, NULL_TOKEN, 0};
    else
      return (TransitionResult){DFA_INITIAL, true, TK_LT, 2};
  }
  case DFA_S29: {
    if (inputChar == '-')
      return (TransitionResult){DFA_INITIAL, true, TK_ASSIGNOP, 0};
    else
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 5};
  }
  case DFA_S33: {
    if (inputChar == '=')
      return (TransitionResult){DFA_INITIAL, true, TK_GE, 0};
    else
      return (TransitionResult){DFA_INITIAL, true, TK_GT, 1};
  }
  case DFA_S37: {
    if ((inputChar >= 'a' && inputChar <= 'z') ||
        (inputChar >= 'A' && inputChar <= 'Z')) {
      return (TransitionResult){DFA_S38, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 6};
    }
  }
  case DFA_S38: {
    if ((inputChar >= 'a' && inputChar <= 'z') ||
        (inputChar >= 'A' && inputChar <= 'Z')) {
      return (TransitionResult){DFA_S38, false, NULL_TOKEN, 0};
    } else if (inputChar >= '0' && inputChar <= '9') {
      return (TransitionResult){DFA_S40, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_FUNID, 1};
    }
  }
  case DFA_S40: {
    if (inputChar >= '0' && inputChar <= '9') {
      return (TransitionResult){DFA_S40, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_FUNID, 1};
    }
  }
  case DFA_S41: {
    if (inputChar >= 'a' && inputChar <= 'z') {
      return (TransitionResult){DFA_S42, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 7};
    }
  }
  case DFA_S42: {
    if (inputChar >= 'a' && inputChar <= 'z') {
      return (TransitionResult){DFA_S42, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_RUID, 1};
    }
  }
  case DFA_S44: {
    if (inputChar >= '0' && inputChar <= '9') {
      return (TransitionResult){DFA_S44, false, NULL_TOKEN, 0};
    } else if (inputChar == '.') {
      return (TransitionResult){DFA_S46, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_NUM, 1};
    }
  }
  case DFA_S46: {
    if (inputChar >= '0' && inputChar <= '9') {
      return (TransitionResult){DFA_S47, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_NUM, 2};
    }
  }
  case DFA_S47: {
    if (inputChar >= '0' && inputChar <= '9') {
      return (TransitionResult){DFA_S48, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 8};
    }
  }
  case DFA_S48: {
    if (inputChar == 'E') {
      return (TransitionResult){DFA_S50, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_RNUM, 1};
    }
  }
  case DFA_S50: {
    if (inputChar == '+' || inputChar == '-') {
      return (TransitionResult){DFA_S51, false, NULL_TOKEN, 0};
    } else if (inputChar >= '0' && inputChar <= '9') {
      return (TransitionResult){DFA_S52, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 9};
    }
  }
  case DFA_S51: {
    if (inputChar >= '0' && inputChar <= '9') {
      return (TransitionResult){DFA_S52, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 10};
    }
  }
  case DFA_S52: {
    if (inputChar >= '0' && inputChar <= '9') {
      return (TransitionResult){DFA_INITIAL, true, TK_RNUM, 0};
    } else {
      return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0, 11};
    }
  }
  case DFA_S55: {
    if (inputChar >= 'a' && inputChar <= 'z') {
      return (TransitionResult){DFA_S55, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_FIELDID, 1};
    }
  }
  case DFA_S57: {
    if (inputChar >= 'a' && inputChar <= 'z') {
      return (TransitionResult){DFA_S55, false, NULL_TOKEN, 0};
    } else if (inputChar >= '2' && inputChar <= '7') {
      return (TransitionResult){DFA_S58, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_FIELDID, 1};
    }
  }
  case DFA_S58: {
    if (inputChar >= '2' && inputChar <= '7') {
      return (TransitionResult){DFA_S59, false, NULL_TOKEN, 0};
    } else if (inputChar >= 'b' && inputChar <= 'd') {
      return (TransitionResult){DFA_S58, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_ID, 1};
    }
  }
  case DFA_S59: {

    if (inputChar >= '2' && inputChar <= '7') {
      return (TransitionResult){DFA_S59, false, NULL_TOKEN, 0};
    } else {
      return (TransitionResult){DFA_INITIAL, true, TK_ID, 1};
    }
  }
  default: {
    // If no match
    return (TransitionResult){DFA_ERROR, false, NULL_TOKEN, 0};
  }
  }
}

/* Trie as lookup table for keywords - grouped by category */
void setupKeywordTable() {
  keywordTable = allocateTrieNode();
  
  /* Control flow keywords */
  insertKeyword("if", keywordTable, TK_IF);
  insertKeyword("then", keywordTable, TK_THEN);
  insertKeyword("else", keywordTable, TK_ELSE);
  insertKeyword("endif", keywordTable, TK_ENDIF);
  insertKeyword("while", keywordTable, TK_WHILE);
  insertKeyword("endwhile", keywordTable, TK_ENDWHILE);
  insertKeyword("return", keywordTable, TK_RETURN);
  insertKeyword("call", keywordTable, TK_CALL);
  insertKeyword("end", keywordTable, TK_END);
  
  /* Type keywords */
  insertKeyword("int", keywordTable, TK_INT);
  insertKeyword("real", keywordTable, TK_REAL);
  insertKeyword("record", keywordTable, TK_RECORD);
  insertKeyword("endrecord", keywordTable, TK_ENDRECORD);
  insertKeyword("union", keywordTable, TK_UNION);
  insertKeyword("endunion", keywordTable, TK_ENDUNION);
  insertKeyword("type", keywordTable, TK_TYPE);
  insertKeyword("definetype", keywordTable, TK_DEFINETYPE);
  
  /* I/O keywords */
  insertKeyword("input", keywordTable, TK_INPUT);
  insertKeyword("output", keywordTable, TK_OUTPUT);
  insertKeyword("read", keywordTable, TK_READ);
  insertKeyword("write", keywordTable, TK_WRITE);
  
  /* Parameter keywords */
  insertKeyword("parameter", keywordTable, TK_PARAMETER);
  insertKeyword("parameters", keywordTable, TK_PARAMETERS);
  insertKeyword("list", keywordTable, TK_LIST);
  insertKeyword("with", keywordTable, TK_WITH);
  
  /* Misc keywords */
  insertKeyword("as", keywordTable, TK_AS);
  insertKeyword("global", keywordTable, TK_GLOBAL);
}

/* Function to print token types from enum - grouped by category */
char *tokenToString(TOKEN_TYPE tokenType) {
  switch (tokenType) {
  /* Arithmetic operators */
  case TK_PLUS:
    return "TK_PLUS";
  case TK_MINUS:
    return "TK_MINUS";
  case TK_MUL:
    return "TK_MUL";
  case TK_DIV:
    return "TK_DIV";
  
  /* Relational operators */
  case TK_LT:
    return "TK_LT";
  case TK_LE:
    return "TK_LE";
  case TK_GT:
    return "TK_GT";
  case TK_GE:
    return "TK_GE";
  case TK_EQ:
    return "TK_EQ";
  case TK_NE:
    return "TK_NE";
  
  /* Logical operators */
  case TK_AND:
    return "TK_AND";
  case TK_OR:
    return "TK_OR";
  case TK_NOT:
    return "TK_NOT";
  
  /* Assignment */
  case TK_ASSIGNOP:
    return "TK_ASSIGNOP";
  
  /* Delimiters */
  case TK_COMMA:
    return "TK_COMMA";
  case TK_SEM:
    return "TK_SEM";
  case TK_COLON:
    return "TK_COLON";
  case TK_DOT:
    return "TK_DOT";
  case TK_OP:
    return "TK_OP";
  case TK_CL:
    return "TK_CL";
  case TK_SQL:
    return "TK_SQL";
  case TK_SQR:
    return "TK_SQR";
  
  /* Identifiers */
  case TK_ID:
    return "TK_ID";
  case TK_FIELDID:
    return "TK_FIELDID";
  case TK_FUNID:
    return "TK_FUNID";
  case TK_RUID:
    return "TK_RUID";
  case TK_MAIN:
    return "TK_MAIN";
  
  /* Literals */
  case TK_NUM:
    return "TK_NUM";
  case TK_RNUM:
    return "TK_RNUM";
  
  /* Type keywords */
  case TK_INT:
    return "TK_INT";
  case TK_REAL:
    return "TK_REAL";
  case TK_RECORD:
    return "TK_RECORD";
  case TK_ENDRECORD:
    return "TK_ENDRECORD";
  case TK_UNION:
    return "TK_UNION";
  case TK_ENDUNION:
    return "TK_ENDUNION";
  case TK_TYPE:
    return "TK_TYPE";
  case TK_DEFINETYPE:
    return "TK_DEFINETYPE";
  case TK_AS:
    return "TK_AS";
  
  /* Control flow keywords */
  case TK_IF:
    return "TK_IF";
  case TK_THEN:
    return "TK_THEN";
  case TK_ELSE:
    return "TK_ELSE";
  case TK_ENDIF:
    return "TK_ENDIF";
  case TK_WHILE:
    return "TK_WHILE";
  case TK_ENDWHILE:
    return "TK_ENDWHILE";
  case TK_END:
    return "TK_END";
  case TK_RETURN:
    return "TK_RETURN";
  case TK_CALL:
    return "TK_CALL";
  
  /* I/O keywords */
  case TK_INPUT:
    return "TK_INPUT";
  case TK_OUTPUT:
    return "TK_OUTPUT";
  case TK_READ:
    return "TK_READ";
  case TK_WRITE:
    return "TK_WRITE";
  
  /* Parameter keywords */
  case TK_PARAMETER:
    return "TK_PARAMETER";
  case TK_PARAMETERS:
    return "TK_PARAMETERS";
  case TK_LIST:
    return "TK_LIST";
  case TK_WITH:
    return "TK_WITH";
  case TK_GLOBAL:
    return "TK_GLOBAL";
  
  /* Special tokens */
  case TK_COMMENT:
    return "TK_COMMENT";
  case EPSILLON:
    return "EPSILLON";
  
  default:
    return "INVALID";
  }
}

/* Helper function to check if buffer boundary was crossed */
static bool crossedBufferBoundary(int prevPos, int currPos) {
  bool wasInFirstHalf = (prevPos < BUFF_CAPACITY);
  bool nowInSecondHalf = (currPos >= BUFF_CAPACITY);
  bool wasInSecondHalf = (prevPos >= BUFF_CAPACITY);
  bool nowInFirstHalf = (currPos < BUFF_CAPACITY);
  
  return (wasInFirstHalf && nowInSecondHalf) || (wasInSecondHalf && nowInFirstHalf);
}

/* Helper function to fill buffer region with nulls from start to end */
static void fillBufferWithNulls(DualBufferPtr bufPtr, int start, int end) {
  int k = start;
  while (k < end) {
    bufPtr->storage[k] = '\0';
    k++;
  }
}

/* Function to populate twin buffer by reading from file based on primary buffer */
void loadBuffer(FILE *sourceFile, DualBufferPtr bufPtr) {
  int startIdx, endIdx;
  
  /* Determine which half of buffer to fill */
  if (bufPtr->currentPos >= BUFF_CAPACITY) {
    startIdx = 0;
    endIdx = BUFF_CAPACITY;
  } else {
    startIdx = BUFF_CAPACITY;
    endIdx = 2 * BUFF_CAPACITY;
  }
  
  /* Fill the appropriate buffer half */
  int idx = startIdx;
  while (idx < endIdx) {
    char ch = fgetc(sourceFile);
    
    if (ch == EOF) {
      fillBufferWithNulls(bufPtr, idx, endIdx);
      break;
    }
    
    bufPtr->storage[idx] = ch;
    idx++;
  }
}

/* Advance buffer position and refill if needed */
static void advanceBufferPosition(FILE *sourceFile, DualBufferPtr bufPtr, int *prevPosPtr) {
  bufPtr->currentPos++;
  bufPtr->currentPos = bufPtr->currentPos % (2 * BUFF_CAPACITY);
  int currPos = bufPtr->currentPos;
  
  if (crossedBufferBoundary(*prevPosPtr, currPos)) {
    loadBuffer(sourceFile, bufPtr);
  }
  
  *prevPosPtr = bufPtr->currentPos;
}

/* Function to handle comments by iterating over buffer until '\n' or '\0' is encountered */
void bypassCommentBlock(FILE *sourceFile, DualBufferPtr bufPtr) {
  int prevPos = bufPtr->currentPos;
  char currentChar = bufPtr->storage[bufPtr->currentPos];
  
  /* Skip until end of line or end of file */
  while (currentChar != '\n' && currentChar != '\0') {
    advanceBufferPosition(sourceFile, bufPtr, &prevPos);
    currentChar = bufPtr->storage[bufPtr->currentPos];
  }
  
  /* Move past the newline/null character */
  advanceBufferPosition(sourceFile, bufPtr, &prevPos);
}

/* Helper function to print error pattern between indices */
static void printErrorPattern(DualBufferPtr bufPtr, int startIdx, int endIdx) {
  int pos = startIdx;
  while (pos != endIdx) {
    printf("%c", bufPtr->storage[pos]);
    pos++;
    pos = pos % (2 * BUFF_CAPACITY);
  }
}

/* Helper function to get error message based on error code */
static const char* getErrorMessage(int errType) {
  if (errType == 1) return ": Expected @@@\n";
  if (errType == 2) return ": Expected !=\n";
  if (errType == 3) return ": Expected &&&\n";
  if (errType == 4) return ": Expected ==\n";
  if (errType == 5) return ": Expected <---\n";
  if (errType == 6) return ": Expected a letter [a-z]|[A-Z] after _\n";
  if (errType == 7) return ": Expected a lowercase letter [a-z] after #\n";
  if (errType == 8) return ": Expected a two digits [0-9] after decimal . but got one\n";
  if (errType == 9) return ": Expected a digit [0-9] or +|- after E\n";
  if (errType == 10) return ": Expected a digit [0-9] after +|-|E\n";
  if (errType == 11) return ": Expected a two digits [0-9] after E|+|- but got one\n";
  return "\n";
}

/* Function to handle invalid errors and print expected pattern based on DFA */
void outputLexicalError(TransitionResult transResult, DualBufferPtr bufPtr, int startIdx, int endIdx) {
  printf("Line %02d: Lexical Error: ", bufPtr->currentLine);
  printf("Error: ");
  
  /* Handle single character error */
  if (startIdx == endIdx) {
    printf("Unknown symbol <%c>\n", bufPtr->storage[startIdx]);
    bufPtr->currentPos = (endIdx + 1) % (2 * BUFF_CAPACITY);
    return;
  }
  
  /* Print the error pattern */
  printf("Unknown pattern <");
  printErrorPattern(bufPtr, startIdx, endIdx);
  printf("> ");
  
  /* Update buffer position and print error message */
  bufPtr->currentPos = endIdx;
  printf("%s", getErrorMessage(transResult.errorCode));
}

/* Helper function to report identifier length error */
static void reportIdentifierLengthError(TokenInfoPtr tokenPtr, const char *idType, int maxLen) {
  printf("Line %02d: Lexical Error: ", tokenPtr->lineNumber);
  printf("Error: %s Identifier %s is longer than the prescribed length of %d characters\n",
         idType, tokenPtr->tokenValue, maxLen);
  free(tokenPtr->tokenValue);
  free(tokenPtr);
}

/* Handle valid errors based on identifier length */
bool validateTokenError(TokenInfoPtr tokenPtr) {
  TOKEN_TYPE category = tokenPtr->tokenCategory;
  int tokenLen = tokenPtr->tokenLength;
  
  if (category == TK_ID && tokenLen > 20) {
    reportIdentifierLengthError(tokenPtr, "Variable", 20);
    return false;
  }
  
  if (category == TK_FUNID && tokenLen > 30) {
    reportIdentifierLengthError(tokenPtr, "Function", 30);
    return false;
  }
  
  return true;
}

// Function to get next token
TokenInfoPtr fetchNextToken(FILE *sourceFile, DualBufferPtr bufPtr) {
  DFA_STATE currState = DFA_INITIAL;
  // If comment
  if (bufPtr->storage[bufPtr->currentPos] == '%') {
    bypassCommentBlock(sourceFile, bufPtr);
    char *tokenText = (char *)malloc(sizeof(char) * 2);
    tokenText[0] = '%';
    tokenText[1] = '\0';
    TokenInfoPtr tokenPtr = (TokenInfoPtr)malloc(sizeof(LexicalToken));
    tokenPtr->tokenValue = tokenText;
    tokenPtr->tokenLength = 1;
    tokenPtr->lineNumber = bufPtr->currentLine;
    tokenPtr->tokenCategory = TK_COMMENT;
    return tokenPtr;
  }
  int startIdx = bufPtr->currentPos;
  int endIdx = bufPtr->currentPos;
  TransitionResult transResult = performDFATransition(currState, bufPtr->storage[startIdx]);
  while (!(transResult.shouldEmitToken || transResult.targetState == DFA_ERROR)) {
    endIdx++;
    endIdx = endIdx % (2 * BUFF_CAPACITY);
    transResult = performDFATransition(transResult.targetState, bufPtr->storage[endIdx]);
  }
  // If invalid state
  if (transResult.targetState == DFA_ERROR) {
    outputLexicalError(transResult, bufPtr, startIdx, endIdx);
    return NULL;
  } else if (transResult.emittedTokenType == BLANK) {
    // If blank
    endIdx++;
    endIdx = endIdx % (2 * BUFF_CAPACITY);
    bufPtr->currentPos = endIdx;
    return NULL;
  } else if (transResult.targetState == NEWLINE) {
    // If newline
    endIdx++;
    endIdx = endIdx % (2 * BUFF_CAPACITY);
    bufPtr->currentPos = endIdx;
    bufPtr->currentLine++;
    return NULL;
  } else {
    int retractCount = transResult.charRetraction;
    endIdx = (endIdx - retractCount + 2 * BUFF_CAPACITY) % (2 * BUFF_CAPACITY);
    int tokenLen = 0;
    if (endIdx >= startIdx) {
      tokenLen = endIdx - startIdx + 1;
    } else {
      tokenLen = 2 * BUFF_CAPACITY - startIdx + endIdx + 1;
    }
    char *tokenText = (char *)malloc(sizeof(char) * (tokenLen + 1));
    int charIdx = 0;
    while (startIdx != endIdx) {
      tokenText[charIdx] = bufPtr->storage[startIdx];
      startIdx++;
      startIdx = startIdx % (2 * BUFF_CAPACITY);
      charIdx++;
    }
    tokenText[charIdx] = bufPtr->storage[startIdx];
    tokenText[charIdx + 1] = '\0';
    startIdx++;
    startIdx = startIdx % (2 * BUFF_CAPACITY);
    TokenInfoPtr tokenPtr = (TokenInfoPtr)malloc(sizeof(LexicalToken));
    tokenPtr->tokenValue = tokenText;
    tokenPtr->tokenLength = tokenLen;
    tokenPtr->lineNumber = bufPtr->currentLine;
    // Keyword search
    if (transResult.emittedTokenType == TK_FIELDID) {
      tokenPtr->tokenCategory = searchKeyword(tokenText, keywordTable);
    } else if (transResult.emittedTokenType == TK_FUNID) {
      // Function Identifier
      if (compareStrings(tokenText, "_main")) {
        tokenPtr->tokenCategory = TK_MAIN;
      } else {
        tokenPtr->tokenCategory = TK_FUNID;
      }
    } else {
      tokenPtr->tokenCategory = transResult.emittedTokenType;
    }
    bufPtr->currentPos = startIdx;
    return tokenPtr;
  }
}

/* Function to print buffer (Debugging purpose) */
void showBufferState(DualBufferPtr bufPtr) {
  int idx = 0;
  while (idx < 2 * BUFF_CAPACITY) {
    printf("%c ", bufPtr->storage[idx]);
    idx++;
  }
  printf("\n");
}

/* Helper function to create end-of-input marker token */
static TokenInfoPtr createDollarToken(void) {
  TokenInfoPtr endMarker = (TokenInfoPtr)malloc(sizeof(LexicalToken));
  endMarker->tokenCategory = DOLLAR;
  return endMarker;
}

/* Helper function to check if token should be reported */
static bool isReportableToken(TokenInfoPtr tokenPtr) {
  TOKEN_TYPE cat = tokenPtr->tokenCategory;
  return (cat != NULL_TOKEN && cat != NEWLINE && cat != EXIT_TOKEN && cat != BLANK);
}

/* Function to get next token as per request from Parser */
TokenInfoPtr retrieveToken(FILE *sourceFile, DualBufferPtr bufPtr) {
  int prevPos = bufPtr->currentPos;
  TokenInfoPtr tokenPtr = fetchNextToken(sourceFile, bufPtr);
  bool validToken = false;
  
  if (tokenPtr != NULL) {
    TOKEN_TYPE category = tokenPtr->tokenCategory;
    
    /* Handle line counting */
    if (category == NEWLINE || category == TK_COMMENT) {
      bufPtr->currentLine++;
    }
    
    /* Handle comment token */
    if (category == TK_COMMENT) {
      if (bufPtr->storage[bufPtr->currentPos] != '\0') {
        return retrieveToken(sourceFile, bufPtr);
      }
      return createDollarToken();
    }
    
    /* Validate reportable tokens */
    if (isReportableToken(tokenPtr) && validateTokenError(tokenPtr)) {
      validToken = true;
    }
  }
  
  /* Check for buffer boundary crossing */
  int currPos = bufPtr->currentPos;
  if (crossedBufferBoundary(prevPos, currPos)) {
    loadBuffer(sourceFile, bufPtr);
  }
  
  /* Return valid token or continue scanning */
  if (validToken) {
    return tokenPtr;
  }
  
  if (bufPtr->storage[bufPtr->currentPos] != '\0') {
    return retrieveToken(sourceFile, bufPtr);
  }
  
  return createDollarToken();
}

/* Helper function to initialize dual buffer */
static void initializeDualBuffer(DualBufferPtr bufPtr, FILE *sourceFile) {
  int idx = 0;
  while (idx < 2 * BUFF_CAPACITY) {
    bufPtr->storage[idx] = '\0';
    idx++;
  }
  bufPtr->currentPos = 2 * BUFF_CAPACITY - 1;
  bufPtr->currentLine = 1;
  loadBuffer(sourceFile, bufPtr);
  bufPtr->currentPos = 0;
  loadBuffer(sourceFile, bufPtr);
}

/* Function to get stream of tokens and print it to console */
void initializeInputStream(FILE *sourceFile) {
  DualBufferPtr bufPtr = (DualBufferPtr)malloc(sizeof(DualBuffer));
  initializeDualBuffer(bufPtr, sourceFile);
  setupKeywordTable();
  
  while (bufPtr->storage[bufPtr->currentPos] != '\0') {
    int prevPos = bufPtr->currentPos;
    TokenInfoPtr tokenPtr = fetchNextToken(sourceFile, bufPtr);
    
    if (tokenPtr != NULL) {
      TOKEN_TYPE category = tokenPtr->tokenCategory;
      
      /* Handle line counting */
      if (category == NEWLINE || category == TK_COMMENT) {
        bufPtr->currentLine++;
      }
      
      /* Print valid tokens */
      if (isReportableToken(tokenPtr) && validateTokenError(tokenPtr)) {
        printf("Line no. %d Lexeme %s Token %s\n", tokenPtr->lineNumber, 
               tokenPtr->tokenValue, tokenToString(tokenPtr->tokenCategory));
      }
      
      /* Skip comments */
      if (category == TK_COMMENT) {
        continue;
      }
    }
    
    /* Check for buffer boundary crossing */
    int currPos = bufPtr->currentPos;
    if (crossedBufferBoundary(prevPos, currPos)) {
      loadBuffer(sourceFile, bufPtr);
    }
  }
  
  free(bufPtr);
}

/* Function to remove comments from source file */
void stripComments(char *inputFilePath) {
  FILE *inputFilePtr = fopen(inputFilePath, "r");

  if (inputFilePtr == NULL) {
    printf("Error: Unable to open testcase file %s\n", inputFilePath);
    return;
  }

  char ch = fgetc(inputFilePtr);
  
  while (ch != EOF) {
    /* Handle comment line */
    if (ch == '%') {
      /* Skip until end of line or end of file */
      while (ch != '\n' && ch != EOF) {
        ch = fgetc(inputFilePtr);
      }
      
      /* Move past newline if present */
      if (ch == '\n') {
        ch = fgetc(inputFilePtr);
      }
      printf("\n");
      continue;
    }
    
    /* Print non-comment characters */
    printf("%c", ch);
    ch = fgetc(inputFilePtr);
  }
  
  fclose(inputFilePtr);
}
