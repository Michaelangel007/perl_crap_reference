// Perl: The Complete Reference (1st Ed.), Chapter 19, Perl Compiler, Page 638, 1999, ISBN: 0-07-212000-2
#include <stdio.h>

char words[32000][260];
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
  long count = 0;

  data = fopen("words.txt","rw");

  while(!feof(data))
    {
      fgets(line,255,data);
      addtolist(line);
    }

  printf( "%ld unique lines\n",wordidx);
}

