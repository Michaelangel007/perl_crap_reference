// Perl: The Complete Reference (1st Ed.), Chapter 19, Perl Compiler, Page 638, 1999, ISBN: 0-07-212000-2
#include <stdio.h>
                                        // *facepalm* Can't even compile due to missing: #include <string.h>

char words[32000][260];                 // Magic numbers. Why 260? Should be cache line size: 256
long wordidx = 0;                       // should be size_t, crappy var names: g_nWords;

void addtolist(char word[255])          // crappy lowercase, arg should be: char *word
{
  int i=0;                              // useless re-init to zero
  for(i=0;i<wordidx;i++)                // no whitespace makes this crap unreadable
    {                                   // idiotic double indentation
      if (strcmp(word,words[i]) == 0)   // missing <string.h>
          return;                       // inconsistent indentation
    }
                                        // Off-by-one bug
  strcpy(words[++wordidx],word);        // missing <string.h> similiar variables: words and word
}

int main()
{
  char line[255];                       // Hard-coded magic number, doesn't match words[][]
  FILE *data;
  long count = 0;                       // useless unused variable

  data = fopen("words.txt","rw");       // Why write mode for reading???
                                        // missing: if( data ) { ... }
  while(!feof(data))
    {
      fgets(line,255,data);             // Hard-coded magic number
      addtolist(line);
    }

  printf( "%ld unique lines\n",wordidx);
                                        // missing: fclose( data );
                                        // missing: return 0;
}

