#include "../headers/defs.h"
#include "../headers/data.h"
#include "../headers/decl.h"

int parse_type() {
    int type;
    switch (Token.token) {
        case T_VOID:
            type = P_VOID;
            break;
        case T_CHAR:
            type = P_CHAR;
            break;
        case T_INT:
            type = P_INT;
            break;
        case T_LONG:
            type = P_LONG;
            break;
        default:
            fatald("Illegal type, token", Token.token);
    }

    while (1) {
        scan(&Token);
        if (Token.token != T_STAR) {
            break;
        }
        type = pointer_to(type);
    }

    return type;
}

void var_declaration(int type) {
    int id;
    while (1) {
        id = addglob(Text, type, S_VARIABLE, 0);
        genglobsym(id);
        if (Token.token == T_SEMI) {
            scan(&Token);
            return;
        }
        else if (Token.token == T_COMMA) {
            scan(&Token);
            ident();
            continue;
        }
        else {
            fatald("Unexpected token in global variable declaration", Token.token);
        }
    }
}

struct ASTnode *function_declaration(int type) {
  struct ASTnode *tree, *finalstmt;
  int nameslot, endlabel;

  endlabel = genlabel();
  nameslot = addglob(Text, type, S_FUNCTION, endlabel);
  Functionid = nameslot;

  lparen();
  rparen();

  tree = compound_statement();

  if (type != P_VOID) {
    if (tree == NULL)
      fatal("No statements in function with non-void type");

    finalstmt = (tree->op == A_GLUE) ? tree->right : tree;
    if (finalstmt == NULL || finalstmt->op != A_RETURN)
      fatal("No return for function with non-void type");
  }
  return (mkastunary(A_FUNCTION, type, tree, nameslot));
}


void global_declarations() {
    struct ASTnode *tree;
    int type;

    while (1) {
        type = parse_type();
        ident();
        if (Token.token == T_LPAREN) {
            tree = function_declaration(type);
            genAST(tree, NOREG, 0);
        } else {
            var_declaration(type);
        }
        if (Token.token == T_EOF) {
            break;
        }
    }
}