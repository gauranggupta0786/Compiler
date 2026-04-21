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
#ifndef LEXER_DEF_HEADER
#define LEXER_DEF_HEADER

#include <stdbool.h>

#define BUFF_CAPACITY 50
#define NUM_DFA_STATES 64
#define NUM_TOKEN_TYPES 63

static const char validCharSet[] = {
    'a', 'b', 'c', 'd',  'e',  'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's',  't',  'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
    'E', 'F', 'G', 'H',  'I',  'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
    'T', 'U', 'V', 'W',  'X',  'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', ' ', '\n', '\t', '~', '(', '[', ']', ')', '<', '>', '!', '@', '#',
    '%', '&', '*', '_',  '+',  '-', '/', '=', ';', ':', '.', ','};
static const int charSetLength = sizeof(validCharSet) / sizeof(validCharSet[0]);

typedef enum DFA_STATE {
  DFA_INITIAL,
  DFA_S1,
  DFA_S2,
  DFA_S3,
  DFA_S4,
  DFA_S5,
  DFA_S6,
  DFA_S7,
  DFA_S8,
  DFA_S9,
  DFA_S10,
  DFA_S11,
  DFA_S12,
  DFA_S13,
  DFA_S14,
  DFA_S15,
  DFA_S16,
  DFA_S17,
  DFA_S18,
  DFA_S19,
  DFA_S20,
  DFA_S21,
  DFA_S22,
  DFA_S23,
  DFA_S24,
  DFA_S25,
  DFA_S26,
  DFA_S27,
  DFA_S28,
  DFA_S29,
  DFA_S30,
  DFA_S31,
  DFA_S32,
  DFA_S33,
  DFA_S34,
  DFA_S35,
  DFA_S36,
  DFA_S37,
  DFA_S38,
  DFA_S39,
  DFA_S40,
  DFA_S41,
  DFA_S42,
  DFA_S43,
  DFA_S44,
  DFA_S45,
  DFA_S46,
  DFA_S47,
  DFA_S48,
  DFA_S49,
  DFA_S50,
  DFA_S51,
  DFA_S52,
  DFA_S53,
  DFA_S54,
  DFA_S55,
  DFA_S56,
  DFA_S57,
  DFA_S58,
  DFA_S59,
  DFA_S60,
  DFA_S61,
  DFA_ERROR,
  DFA_FINALIZE,

} DFA_STATE;

typedef enum TOKEN_TYPE {
  TK_ASSIGNOP,
  TK_COMMENT,
  TK_FIELDID,
  TK_ID,
  TK_NUM,
  TK_RNUM,
  TK_FUNID,
  TK_RUID,
  TK_WITH,
  TK_PARAMETERS,
  TK_END,
  TK_WHILE,
  TK_UNION,
  TK_ENDUNION,
  TK_DEFINETYPE,
  TK_AS,
  TK_TYPE,
  TK_MAIN,
  TK_GLOBAL,
  TK_PARAMETER,
  TK_LIST,
  TK_SQL,
  TK_SQR,
  TK_INPUT,
  TK_OUTPUT,
  TK_INT,
  TK_REAL,
  TK_COMMA,
  TK_SEM,
  TK_COLON,
  TK_DOT,
  TK_ENDWHILE,
  TK_OP,
  TK_CL,
  TK_IF,
  TK_THEN,
  TK_ENDIF,
  TK_READ,
  TK_WRITE,
  TK_RETURN,
  TK_PLUS,
  TK_MINUS,
  TK_MUL,
  TK_DIV,
  TK_CALL,
  TK_RECORD,
  TK_ENDRECORD,
  TK_ELSE,
  TK_AND,
  TK_OR,
  TK_NOT,
  TK_LT,
  TK_LE,
  TK_EQ,
  TK_GT,
  TK_GE,
  TK_NE,
  NULL_TOKEN,
  NEWLINE,
  EXIT_TOKEN,
  BLANK,
  EPSILLON, // Used in FirstFollow computation
  DOLLAR,   // Used in FirstFollow computation
} TOKEN_TYPE;

typedef struct LexicalToken {
  TOKEN_TYPE tokenCategory;
  char *tokenValue;
  int tokenLength;
  int lineNumber;
} LexicalToken;


typedef LexicalToken *TokenInfoPtr;

typedef struct TransitionResult {
  DFA_STATE targetState;
  bool shouldEmitToken;
  TOKEN_TYPE emittedTokenType;
  int charRetraction;
  int errorCode;
} TransitionResult;

typedef struct DualBuffer {
  char storage[2*BUFF_CAPACITY];
  int currentPos;
  int currentLine;
} DualBuffer;

typedef DualBuffer *DualBufferPtr;

#endif
