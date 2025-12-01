#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TEXTLEN		512	


enum {
  T_EOF,
  T_PLUS, T_MINUS,
  T_STAR, T_SLASH,
  T_EQ, T_NE,
  T_LT, T_GT, T_LE, T_GE,
  T_INTLIT, T_SEMI, T_ASSIGN, T_IDENT,
  T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
  T_PRINT, T_INT, T_IF, T_ELSE, T_WHILE,
  T_FOR, T_VOID, T_CHAR
};


struct token {
  int token;			
  int intvalue;			
};


enum {
  A_ADD=1, A_SUBTRACT, A_MULTIPLY, A_DIVIDE,
  A_EQ, A_NE,
  A_LT, A_GT, A_LE, A_GE,
  A_INTLIT,
  A_LVIDENT, A_ASSIGN, A_IDENT, A_PRINT, 
  A_GLUE, A_IF, A_WHILE,
  A_FUNCTION, A_WIDEN
};

enum {
  P_NONE, P_VOID, P_CHAR, P_INT
};

enum {
  S_VARIABLE, S_FUNCTION
};

struct ASTnode {
  int op;			
  int type;
  struct ASTnode *left;		
  struct ASTnode *mid;
  struct ASTnode *right;
  union {
    int intvalue;
    int id;
  } v;	
};

#define NOREG   -1

struct symTable {
  char *name;
  int type;
  int stype;
};