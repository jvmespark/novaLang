#include "../headers/defs.h"
#define extern_
#include "../headers/data.h"
#undef extern_
#include "../headers/decl.h"
#include <errno.h>

static void init() {
  Line = 1;
  Putback = '\n';
}


static void usage(char *prog) {
  fprintf(stderr, "Usage: %s infile\n", prog);
  exit(1);
}

void main(int argc, char *argv[]) {
  struct ASTnode *tree;

  if (argc != 2)
    usage(argv[0]);

  init();
  
  if ((Infile = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
    exit(1);
  }
  
  if ((Outfile = fopen("out.s", "w")) == NULL) {
    fprintf(stderr, "Unable to create out.s: %s\n", strerror(errno));
    exit(1);
  }

  scan(&Token);
  genpreamble();		
  global_declarations();
  genpostamble();
  fclose(Outfile);		
  exit(0);
}
