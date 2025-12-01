#include "../headers/defs.h"
#include "../headers/data.h"
#include "../headers/decl.h"

static int label(void) {
  static int id = 1;
  return id++;
}

static int genIFAST(struct ASTnode *n) {
  int label_false, label_end;
  label_false = label();

  if (n->right)
    label_end = label();

  genAST(n->left, label_false, n->op);
  genfreeregs();

  genAST(n->mid, NOREG, n->op);
  genfreeregs();

  if (n->right)
    cgjump(label_end);
  cglabel(label_false);

  if (n->right) {
    genAST(n->right, NOREG, n->op);
    genfreeregs();
    cglabel(label_end);
  }

  return NOREG;
}

static int genWHILE(struct ASTnode *n) {
  int label_start, label_end;
  label_start = label();
  label_end = label();
  cglabel(label_start);

  genAST(n->left, label_end, n->op);
  genfreeregs();

  genAST(n->right, NOREG, n->op);
  genfreeregs();

  cgjump(label_start);
  cglabel(label_end);
  return NOREG;
}

int genAST(struct ASTnode *n, int reg, int parentASTop) {
  int leftreg, rightreg;

  switch (n->op) {
    case A_IF:
      return genIFAST(n);
    case A_GLUE:
      genAST(n->left, NOREG, n->op);
      genfreeregs();
      genAST(n->right, NOREG, n->op);
      genfreeregs();
      return NOREG;
    case A_WHILE:
      return genWHILE(n);
    case A_FUNCTION:
      cgfuncpreamble(Gsym[n->v.id].name);
      genAST(n->left, NOREG, n->op);
      cgfuncpostamble();
      return (NOREG);
  }
  
  if (n->left)
    leftreg = genAST(n->left, NOREG, n->op);
  if (n->right)
    rightreg = genAST(n->right, leftreg, n->op);

  switch (n->op) {
    case A_ADD:
      return cgadd(leftreg, rightreg);
    case A_SUBTRACT:
      return cgsub(leftreg, rightreg);
    case A_MULTIPLY:
      return cgmul(leftreg, rightreg);
    case A_DIVIDE:
      return cgdiv(leftreg, rightreg);

    case A_EQ:
    case A_NE:
    case A_LT:
    case A_GT:
    case A_LE:
    case A_GE:
      if (parentASTop == A_IF || parentASTop == A_WHILE)
        return cgcompare_and_jump(n->op, leftreg, rightreg, reg);
      else
        return cgcompare_and_set(n->op, leftreg, rightreg);

    case A_INTLIT:
      return (cgloadint(n->v.intvalue));
    case A_IDENT:
      return (cgloadglob(Gsym[n->v.id].name));
    case A_LVIDENT:
      return (cgstorglob(reg, Gsym[n->v.id].name));
    case A_ASSIGN:
      return (rightreg);
    case A_PRINT:
      genprintint(leftreg);
      genfreeregs();
      return (NOREG);

    default:
      fatald("Unknown AST operator", n->op);
  }
}

void genpreamble() {
  cgpreamble();
}
void genfreeregs() {
  freeall_registers();
}
void genprintint(int reg) {
  cgprintint(reg);
}
