#include "../headers/defs.h"
#include "../headers/data.h"
#include "../headers/decl.h"

static int chrpos(char *s, int c) {
  char *p;

  p = strchr(s, c);
  return p ? p - s : -1;
}


static int next(void) {
  int c;

  if (Putback) {		
    c = Putback;		
    Putback = 0;
    return c;
  }

  c = fgetc(Infile);		
  if ('\n' == c)
    Line++;			
  return c;
}

static void putback(int c) {
  Putback = c;
}


static int skip(void) {
  int c;

  c = next();
  while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c) {
    c = next();
  }
  return c;
}


static int scanint(int c) {
  int k, val = 0;

  
  while ((k = chrpos("0123456789", c)) >= 0) {
    val = val * 10 + k;
    c = next();
  }

  
  putback(c);
  return val;
}


static int scanident(int c, char *buf, int lim) {
  int i = 0;

  while (isalpha(c) || isdigit(c) || '_' == c) {
    
    if (lim - 1 == i) {
      printf("identifier too long on line %d\n", Line);
      exit(1);
    } else if (i < lim - 1) {
      buf[i++] = c;
    }
    c = next();
  }
  
  
  putback(c);
  buf[i] = '\0';
  return i;
}


static int keyword(char *s) {
  switch (*s) {
    case 'e':
      if (!strcmp(s, "else"))
        return T_ELSE;
      break;
    case 'p':
      if (!strcmp(s, "print"))
        return T_PRINT;
      break;
    case 'i':
      if (!strcmp(s, "int"))
        return T_INT;
      if (!strcmp(s, "if"))
        return T_IF;
      break;
    case 'w':
      if (!strcmp(s, "while"))
        return T_WHILE;
      break;
    case 'f':
      if (!strcmp(s, "for"))
        return T_FOR;
      break;
    case 'v':
      if (!strcmp(s, "void"))
        return (T_VOID);
      break;
  }
  return 0;
}

static struct token *RejectToken = NULL;

void reject_token(struct token *t) {
  if (RejectToken != NULL) {
    fatal("Can't reject token twice");
  }
  RejectToken = t;
}


int scan(struct token *t) {
  int c, tokentype;

  if (RejectToken != NULL) {
    t = RejectToken;
    RejectToken = NULL;
    return 1;
  }

  c = skip();

  switch (c) {
    case EOF:
      t->token = T_EOF;
      return 0;
    case '+':
      t->token = T_PLUS;
      break;
    case '-':
      t->token = T_MINUS;
      break;
    case '*':
      t->token = T_STAR;
      break;
    case '/':
      t->token = T_SLASH;
      break;
    case ';':
      t->token = T_SEMI;
      break;
    case '{':
      t->token = T_LBRACE;
      break;
    case '}':
      t->token = T_RBRACE;
      break;
    case '(':
      t->token = T_LPAREN;
      break;
    case ')':
      t->token = T_RPAREN;
      break;
    case '=':
      c = next();
      if (c == '=') {
        t->token = T_EQ;
      }
      else {
        putback(c);
        t->token = T_ASSIGN;
      }
      break;
    case '!':
      c = next();
      if (c == '=') {
        t->token = T_NE;
      }
      else {
        fatalc("Unrecognized character", c);
      }
      break;
    case '<':
      c = next();
      if (c == '=') {
        t->token = T_LE;
      }
      else {
        putback(c);
        t->token = T_LT;
      }
      break;
    case '>':
      c = next();
      if (c == '=') {
        t->token = T_GE;
      }
      else {
        putback(c);
        t->token = T_GT;
      }
      break;
    case '&':
      if ((c == next()) == '&') {
        t->token = T_LOGAND;
      } else {
        putback(c);
        t->token = T_AMPER;
      }
      break;
    default:
      if (isdigit(c)) {
        t->intvalue = scanint(c);
        t->token = T_INTLIT;
        break;
      } 
      else if (isalpha(c) || '_' == c) {
        scanident(c, Text, TEXTLEN);

        if (tokentype = keyword(Text)) {
          t->token = tokentype;
          break;
        }

        // not a keyword, set it as an identifier
        t->token = T_IDENT;
        break;
      }
      
      printf("Unrecognised character %c on line %d\n", c, Line);
      exit(1);
  }

  return 1;
}