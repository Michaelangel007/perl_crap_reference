/*

Optimize 3

* Read entire file into memory
* Inlined FNV1A
* Linear Search API

*/

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

INLINE int find_key( uint32_t key )
{
    int i;

    for( i = 0; i < gnWords; ++i )
        if( gaWords[ i ] == key )
            return i+1;
    return 0;
}

INLINE void add_key( uint32_t key )
{
    gaWords[ gnWords++ ] = key;
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

        if( !find_key( hash ) )
            add_key( hash );

        p++; // skip LF = 0x0A
    } while( *p );

//  printf( "Allocated: %lu bytes\n", (unsigned long) size );
    printf( "%ld unique lines\n", gnWords );

    return 0;
}

