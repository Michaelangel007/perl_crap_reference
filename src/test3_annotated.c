// Perl: The Complete Reference (1st Ed.), Chapter 19, Perl Compiler, Page 638, 1999, ISBN: 0-07-212000-2
#include <stdio.h>
                                        // *facepalm* Can't even compile due to missing: #include <string.h>

char words[32000][260];                 // Magic numbers. Why 260? Should be cache line size: 256
long wordidx = 0;                       // should be size_t, crappy _global_ var should have prefix: g_*;
                                        // arg should be: char *word
void addtolist(char word[255])          // no whitespace makes this crap unreadable: add_to_list() OR AddToList()
{
  int i=0;                              // useless re-init to zero
  for(i=0;i<wordidx;i++)                // no whitespace makes this crap unreadable: for( i = 0; i < wordidx; i++ )
    {                                   // idiotic double indentation
      if (strcmp(word,words[i]) == 0)   // missing <string.h>
          return;                       // inconsistent indentation -- why not 2 ???
    }
                                        // Off-by-one bug! Should be post-increment
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

