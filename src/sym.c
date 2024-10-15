#include "../headers/defs.h"
#include "../headers/data.h"
#include "../headers/decl.h"

static int Globs = 0;

int findglob(char *s) {
    for (int i = 0; i < Globs; i++) {
        if (*s == *Gsym[i].name && !strcmp(s, Gsym[i].name))
            return i;
    }
    return -1;
}

static int newglob() {
  int p;

  if ((p = Globs++) >= NSYMBOLS)
    fatal("Too many global symbols");
  return p;
}

int addglob(char *name) {
    int i = findglob(name);
    if (i != -1)
     return i;
    i = newglob();
    Gsym[i].name = strdup(name);
    return i;
}