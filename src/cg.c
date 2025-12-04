#include "../headers/defs.h"
#include "../headers/data.h"
#include "../headers/decl.h"

// x86_64 code generator for NASM

static int freereg[4];
static char *reglist[4] = { "r8", "r9", "r10", "r11" };
static char *breglist[4] = { "r8b", "r9b", "r10b", "r11b" };
static char *dreglist[4] = { "r8d", "r9d", "r10d", "r11d" };
static char *cmplist[] = { "sete", "setne", "setl", "setg", "setle", "setge" };
static char *invcmplist[] = { "jne", "je", "jge", "jle", "jg", "jl" };

void freeall_registers(void) {
  freereg[0] = freereg[1] = freereg[2] = freereg[3] = 1;
}


static int alloc_register(void) {
  for (int i = 0; i < 4; i++) {
    if (freereg[i]) {
      freereg[i] = 0;
      return i;
    }
  }
  fprintf(stderr, "Out of registers!\n");
  exit(1);
}


static void free_register(int reg) {
  if (freereg[reg] != 0) {
    fprintf(stderr, "Error trying to free register %d\n", reg);
    exit(1);
  }
  freereg[reg] = 1;
}

void cgpreamble() {
  freeall_registers();
  fputs("\tglobal\tmain\n"
	"\textern\tprintf\n"
	"\tsection\t.text\n"
	"LC0:\tdb\t\"%d\",10,0\n"
	"printint:\n"
	"\tpush\trbp\n"
	"\tmov\trbp, rsp\n"
	"\tsub\trsp, 16\n"
	"\tmov\t[rbp-4], edi\n"
	"\tmov\teax, [rbp-4]\n"
	"\tmov\tesi, eax\n"
	"\tlea	rdi, [rel LC0]\n"
	"\tmov	eax, 0\n"
	"\tcall	printf\n" "\tnop\n" "\tleave\n" "\tret\n" "\n", Outfile);
}

void cgfuncpreamble(char *name) {
  fprintf(Outfile,
	  "\tsection\t.text\n"
	  "\tglobal\t%s\n"
	  "%s:\n" "\tpush\trbp\n"
	  "\tmov\trbp, rsp\n", name, name);
}

void cgfuncpostamble(int id) {
  cglabel(Gsym[id].endlabel);
  fputs("\tpop	rbp\n" "\tret\n", Outfile);
}


int cgload(int value) {
  int r = alloc_register();

  fprintf(Outfile, "\tmov\t%s, %d\n", reglist[r], value);
  return r;
}


int cgadd(int r1, int r2) {
  fprintf(Outfile, "\tadd\t%s, %s\n", reglist[r2], reglist[r1]);
  free_register(r1);
  return r2;
}


int cgsub(int r1, int r2) {
  fprintf(Outfile, "\tsub\t%s, %s\n", reglist[r1], reglist[r2]);
  free_register(r2);
  return r1;
}


int cgmul(int r1, int r2) {
  fprintf(Outfile, "\timul\t%s, %s\n", reglist[r2], reglist[r1]);
  free_register(r1);
  return r2;
}


int cgdiv(int r1, int r2) {
  fprintf(Outfile, "\tmov\trax, %s\n", reglist[r1]);
  fprintf(Outfile, "\tcqo\n");
  fprintf(Outfile, "\tidiv\t%s\n", reglist[r2]);
  fprintf(Outfile, "\tmov\t%s, rax\n", reglist[r1]);
  free_register(r2);
  return r1;
}


void cgprintint(int r) {
  fprintf(Outfile, "\tmov\trdi, %s\n", reglist[r]);
  fprintf(Outfile, "\tcall\tprintint\n");
  free_register(r);
}

int cgloadint(int value) {
  int r = alloc_register();

  fprintf(Outfile, "\tmov\t%s, %d\n", reglist[r], value);
  return r;
}

int cgloadglob(int id) {
  int r = alloc_register();

  switch (Gsym[id].type) {
    case P_CHAR:
      fprintf(Outfile, "\tmovzx\t%s, byte [%s]\n", reglist[r], 
              Gsym[id].name);
      break;
    case P_INT:
      fprintf(Outfile, "\txor\t%s, %s\n", reglist[r], reglist[r]);
      fprintf(Outfile, "\tmov\t%s, dword [%s]\n", dreglist[r], 
              Gsym[id].name);
      break;
    case P_LONG:
      fprintf(Outfile, "\tmov\t%s, [%s]\n", reglist[r], Gsym[id].name);
      break;
    default:
      fatald("Bad type in cgloadglob:", Gsym[id].type);
  }
  return r;
}

int cgstorglob(int r, int id) {
  switch (Gsym[id].type) {
    case P_CHAR:
      fprintf(Outfile, "\tmov\t[%s], %s\n", Gsym[id].name, breglist[r]);
      break;
    case P_INT:
      fprintf(Outfile, "\tmov\t[%s], %s\n", Gsym[id].name, dreglist[r]);
      break;
    case P_LONG:
      fprintf(Outfile, "\tmov\t[%s], %s\n", Gsym[id].name, reglist[r]);
      break;
    default:
      fatald("Bad type in cgloadglob:", Gsym[id].type);
  }
  return r;
}

void cgglobsym(int id) {
  int typesize;
  typesize = cgprimsize(Gsym[id].type);
  fprintf(Outfile, "\tcommon\t%s %d:%d\n", Gsym[id].name, typesize, typesize);
}

int cgwiden(int r, int oldtype, int newtype) {
  return r;
}

void genglobsym(int id) {
  cgglobsym(id);
}

void cglabel(int l) {
  fprintf(Outfile, "L%d:\n", l);
}

void cgjump(int l) {
  fprintf(Outfile, "\tjmp\tL%d\n", l);
}

int cgcompare_and_set(int ASTop, int r1, int r2) {
  if (ASTop < A_EQ || ASTop > A_GE)
    fatal("Bad ASTop in cgcompare_and_set()");

  fprintf(Outfile, "\tcmp\t%s, %s\n", reglist[r1], reglist[r2]);
  fprintf(Outfile, "\t%s\t%s\n", cmplist[ASTop - A_EQ], breglist[r2]);
  fprintf(Outfile, "\tmovzx\t%s, %s\n", reglist[r2], breglist[r2]);
  free_register(r1);
  return r2;
}

int cgcompare_and_jump(int ASTop, int r1, int r2, int label) {
  if (ASTop < A_EQ || ASTop > A_GE)
    fatal("Bad ASTop in cgcompare_and_jump()");

  fprintf(Outfile, "\tcmp\t%s, %s\n", reglist[r1], reglist[r2]);
  fprintf(Outfile, "\t%s\tL%d\n", invcmplist[ASTop - A_EQ], label);
  freeall_registers();
  return NOREG;
}

static int psize[] = {0, 0, 1, 4, 8};

int cgprimsize(int type) {
  if (type < P_NONE || type > P_LONG) {
    fatal("Bad type in cgprimsize()");
  }
  return psize[type];
}

int cgcall(int r, int id) {
  int outr = alloc_register();
  fprintf(Outfile, "\tmov\trdi, %s\n", reglist[r]);
  fprintf(Outfile, "\tcall\t%s\n", Gsym[id].name);
  fprintf(Outfile, "\tmov\t%s, rax\n", reglist[outr]);
  free_register(r);
  return outr;
}

void cgreturn(int reg, int id) {
  switch (Gsym[id].type) {
    case P_CHAR:
      fprintf(Outfile, "\tmovzx\teax, %s\n", breglist[reg]);
      break;
    case P_INT:
      fprintf(Outfile, "\tmov\teax, %s\n", dreglist[reg]);
      break;
    case P_LONG:
      fprintf(Outfile, "\tmov\trax, %s\n", reglist[reg]);
      break;
    default:
      fatald("Bad function type in cgreturn:", Gsym[id].type);
  }
  cgjump(Gsym[id].endlabel);
}

int cgaddress(int id) {
  int r = alloc_register();
  fprintf(Outfile, "\tmov\t%s, %s\n", reglist[r], Gsym[id].name);
  return r;
}

int cgderef(int r, int type) {
  switch (type) {
    case P_CHARPTR:
      fprintf(Outfile, "\tmovezx\t%s, bytes [%s]\n", reglist[r], reglist[r]);
      break;
    case P_INTPTR:
      fprintf(Outfile, "\tmovzx%s, word [%s]\n", reglist[r], reglist[r]);
      break;
    case P_LONGPTR:
      fprintf(Outfile, "\tmove\t%s, [%s]\n", reglist[r], reglist[r]);
      break;
  }
  return r;
}