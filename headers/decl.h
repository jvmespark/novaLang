int scan(struct token *t);

// tree.c
struct ASTnode *mkastnode(int op, int type, struct ASTnode *left, struct ASTnode *mid, struct ASTnode *right, int intvalue);
struct ASTnode *mkastleaf(int op, int type, int intvalue);
struct ASTnode *mkastunary(int op, int type, struct ASTnode *left, int intvalue);

// gen.c
static int label(void);
static int genIFAST(struct ASTnode *n);
int genAST(struct ASTnode *n, int reg, int parentASTop);
static int genWHILE(struct ASTnode *n);
void genpreamble();
void genpostamble();
void genfreeregs();
void genprintint(int reg);
void genglobsym(int id);

// cg.c
void freeall_registers(void);
void cgpreamble();
void cgfuncpreamble(char *name);
void cgfuncpostamble();
int cgloadint(int value);
int cgloadglob(int identifier);
int cgadd(int r1, int r2);
int cgsub(int r1, int r2);
int cgmul(int r1, int r2);
int cgdiv(int r1, int r2);
void cgprintint(int r);
int cgstorglob(int r, int identifier);
void cgglobsym(int identifier);

void cglabel(int l);
void cgjump(int l);
int cgcompare_and_set(int ASTop, int r1, int r2);
int cgcompare_and_jump(int ASTop, int r1, int r2, int label);
int cgwiden(int r, int oldtype, int newtype);

int cgcall(int r, int id);
int cgprimsize(int type);
void cgreturn(int reg, int id);

// expr.c
struct ASTnode *binexpr(int ptp);
struct ASTnode *funccall(void);

// stmt.c
struct ASTnode *compound_statement(void);
static struct ASTnode *return_statement(void);

// misc.c
void match(int t, char *what);
void semi(void);
void ident(void);
void lbrace(void);
void rbrace(void);
void lparen(void);
void rparen(void);
void fatal(char *s);
void fatals(char *s1, char *s2);
void fatald(char *s, int d);
void fatalc(char *s, int c);

// sym.c
int findglob(char *s);
int addglob(char *name, int type, int stype);

// decl.c
void var_declaration(void);
struct ASTnode *function_declaration(void);

int type_compatible(int *left, int *right, int onlyright);

void reject_token(struct token *t);