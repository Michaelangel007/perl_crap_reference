/*

Optimized 5

* Read entire file into memory
* Inlined FNV1A
* Inlined BinarySearchInsert

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

//  printf( "Allocated: %lu bytes\n", (unsigned long) size );
    printf( "%ld unique lines\n", gnWords );

    return 0;
}

