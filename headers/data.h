#ifndef extern_
#define extern_ extern
#endif

extern_ int Line;			
extern_ int Putback;			
extern_ FILE *Infile;			
extern_ FILE *Outfile;
extern_ struct token Token;		
extern_ char Text[TEXTLEN + 1];		
#define NSYMBOLS    1024
extern_ struct symTable Gsym[NSYMBOLS]; //global symbol table
static int glob = 0; // global symbol slot
extern_ int Functionid;