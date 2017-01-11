// Perl: The Complete Reference, Page 638, 1999, ISBN: 0-07-212000-2
#include <stdio.h>
#include <string.h> // Added include file so it compiles

char words[32000][260]; // 8,320,000 bytes
long wordidx = 0;

void addtolist(char word[255])
{
  int i=0;
  for(i=0;i<wordidx;i++)
    {
      if (strcmp(word,words[i]) == 0)
          return;
    }

  strcpy(words[++wordidx],word);
}

int main()
{
  char line[255];
  FILE *data;
//long count = 0; // removed useless unused var
//printf( "Allocated: %lu bytes\n", (unsigned long) sizeof( words ) );

  data = fopen("words.txt","rw");

  while(!feof(data))
    {
      fgets(line,255,data);
      addtolist(line);
    }

  printf( "%ld unique lines\n",wordidx);

  return 0;
}

