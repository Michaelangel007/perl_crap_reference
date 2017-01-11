# Perl: The Complete (Crap) Reference

In [Perl: The Complete Reference](https://www.amazon.com/Perl-Complete-Reference-Martin-Brown/dp/0072120002/),
Chapter 19, _Perl Compiler_, Page 638, the author compares the speed of Perl with C.
He gives various small examples:

* count the number of lines (test2)
* count the number of unique lines (test3)

both in C and Perl.

Here is the `test3` Perl program:

```Perl
    open(DATA,"<words.txt") || die "Can't open file";

    my %words;

    while(<DATA>)
    {
      $words{$_} = 1;
    }
    close(DATA);

    print scalar keys %words," unique lines\n";
```

Here is the author's "equivalent" C version:

```C
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
```

*facepalm*


# Bugs

How many **noob mistakes** can you spot?

There are _numerous_ things wrong with the C version:

* The bloody thing doens't even compile !
* Has an off-by-one bug!

Here is the full list of everthing wrong:

* [src/test3_annotated.c](src/test3_annotated.c)



# Apples to Oranges

Table 19-8 has benchmark times showing that the Perl version is faster then the C version.

|Operation                                 | Test 3|
|:-----------------------------------------|--------:|
|Compile C program once, execute 100 times | 1103.75 |
|Execute interpreted Perl script 100 times |  197.00 |

First, where is the _data file_ to independently verify the timing???

Second, except this an _apples-to-oranges_comparision -- **different algorithms** are used!
Of course the Perl version is faster.  **No shit, Sherlock.** 
Obviously, the _better algorithm_ in a slower language
is going to "win" against the _worse algorithm_ in a faster language!

The author gives the _excuse:_

   We could have used a binary tree or even a hashing system,
   but that would have added significantly more liens to an already long C program.

1. _No one give a shit how many lines of code C is_ -- the **entire** point of using
  **C is for speed;** not some _bullshit metric!_

Does that make [-2,000 Lines of Code](http://www.folklore.org/StoryView.py?project=Macintosh&story=Negative_2000_Lines_Of_Code.txt&sortOrder=Sort+by+Date&topic=Management) even better??

2. Apparently 34 lines of code is long??? Holy shit! Wait till the author finds
  out how this _toy example_ compares in the _Real World (TM)_ where there are
  _hundreds of thousands of lines of code in a project._

  Apparently including an additional 38 lines for a total of 72 lines was too much work !

/sarcasm _"Hey, that would have been twice the work, right?"_



# Optimization

There are numerous optimizations that can be done:

1. Read entire file into memory
2. Compute FNV1a hash on-the-fly

```C++
    const uint32_t FNV1A_PRIME = 0x01000193; //   16777619
    const uint32_t FNV1A_SEED  = 0x811C9DC5; // 2166136261

    inline uint32_t fnv1a( unsigned char byte, uint32_t hash = FNV1A_SEED )
    {
          return (byte ^ hash) * FNV1A_PRIME;
    }
```

3\. Replace _Linear_ search with _Binary Seach_

Here is an optimized 72 LOC version:

```
    #include <stdio.h>    // printf()
    #include <string.h>   // memmove()
    #include <stdint.h>   // uint32_t
    #include <stdlib.h>   // malloc()
    #include <sys/stat.h> // stat()
    #include "inline.h"   // INLINE

    const uint32_t FNV1A_PRIME = 0x01000193; //   16777619
    const uint32_t FNV1A_SEED  = 0x811C9DC5; // 2166136261

    #define MAX_WORDS  80000

    uint32_t gaWords[ MAX_WORDS ]; // 320,000 bytes
    size_t   gnWords = 0;

    // If the key is  found, returns position > 0 where key was found
    // If the key not found, returns -1 but with key inserted
    INLINE int find_key_insert( uint32_t key )
    {
        int min = 0;
        int max = gnWords;
        int mid = 0;

        while( min <= max )
        {
            mid = (min + max) >> 1;

            /**/ if( gaWords[ mid ] == key )   return mid+1;
            else if( gaWords[ mid ] >  key )   max = mid-1;
            else /*                 <  key )*/ min = mid+1;
        }

        /* */ uint32_t *src = &gaWords[ mid ];
        /* */ uint32_t *dst = src + 1;
        const size_t    len = gnWords - mid;

        memmove( dst, src, sizeof( uint32_t ) * len ); // memcpy() can't alias (overlap)
        gaWords[ mid ] = key;
        gnWords++;

        return -1;
    }

    int main()
    {
        const char *filename = "words.txt";
        struct stat info;

        FILE  *data = fopen( filename, "rb" );
        size_t size = stat( filename, &info ) ? 0 : (size_t) info.st_size;

        char    *p  = (char*) malloc( size+1 );
        uint32_t hash;

        fread( p, size, 1, data );
        p[ size ] = 0;

        do
        {
            hash = FNV1A_SEED;
            while( *p > 0x20 )
                hash =  (*p++ ^ hash) * FNV1A_PRIME;

            find_key_insert( hash );

            p++; // skip LF = 0x0A
        } while( *p );

        printf( "%ld unique lines\n", gnWords );

        return 0;
    }
```

# Benchmarks

Using a dictinary of 79,339 words from [data/words.txt](data/words.txt):

|Description           | Time   |
|:---------------------|-------:|
|Perl Test3 version    | 0.060s |
|Fixed C Test3 v1      | 0.906s |
|Optimized C Test3 v5  | 0.182s |



# Data

I used this word list:


## Wordlist

* http://www.puzzlers.org/pub/wordlists/ospd.txt

There are also public dictionaries one could use:


## Dictionary

* http://foldoc.org/Dictionary.txt

* http://dreamsteep.com/downloads/word-games-and-wordsmith-utilities/120-the-english-open-word-list-eowl/file.html

NOTE: The .html file is actually a .zip file !

```
wget -O eowl.zip http://dreamsteep.com/downloads/word-games-and-wordsmith-utilities/120-the-english-open-word-list-eowl/file.html
```


# References

* http://stackoverflow.com/questions/6441975/where-can-i-download-english-dictionary-database-in-a-text-fomat

