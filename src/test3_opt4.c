/*
https://github.com/Michaelangel007/perl_crap_reference

Optimize 4

* Read entire file into memory
* Use inlined FNV1A
* Binary Search, Insertion API

*/

    #include <stdio.h>    // printf()
    #include <string.h>   // memmove()
    #include <stdint.h>   // uint32_t
    #include <stdlib.h>   // malloc()
    #include <sys/stat.h> // stat()

    #include "inline.h"   // INLINE

    const uint32_t FNV1A_PRIME = 0x01000193; //   16777619
    const uint32_t FNV1A_SEED  = 0x811C9DC5; // 2166136261

    #define LF  0x0A

    #define MAX_WORDS  80000

    size_t   gnWords = 0;
    uint32_t gaWords[ MAX_WORDS ]; // 320,000 bytes

// If the key is found , returns position > 0 where key was found
// If the key not found, returns position < 0 of last best location for insert
// ========================================================================
INLINE int find_key( uint32_t key )
{
    int min = 0;
    int mid = 0;
    int max = gnWords - 1;

    while( min <= max )
    {
        mid = (min + max) >> 1;

        /**/ if( gaWords[ mid ] == key )   return mid+1; // Normal BinSearch returns true
        else if( gaWords[ mid ] >  key )   max =  mid-1;
        else /*                 <  key )*/ min =  mid+1;
    }

    return -(mid+1);
}

// ========================================================================
INLINE void insert_key( uint32_t key, int pos )
{
    int mid = pos - 1;

    while( (mid < gnWords) && (key > gaWords[ mid ]) )
        mid++;

    /* */ uint32_t *src = &gaWords[ mid ];
    /* */ uint32_t *dst = src + 1;
    const size_t    len = gnWords - mid;

    memmove( dst, src, sizeof( uint32_t ) * len ); // memcpy() can't alias (overlap)

    gaWords[ mid ] = key;
    gnWords++;
}

// ========================================================================
int main()
{
    const char *filename = "words.txt";
    struct stat info;

    FILE  *data = fopen( filename, "rb" );
    size_t size = stat( filename, &info ) ? 0 : (size_t) info.st_size;

    unsigned char *buf = (unsigned char*) malloc( size+1 );
    uint32_t hash;
    int      found;

    if( data )
    {
        fread( buf, size, 1, data );
        buf[ size ] = 0;
        fclose( data );

        unsigned char *p = buf;

        do
        {
            hash = FNV1A_SEED;
            while( *p != LF )
                hash = (*p++ ^ hash) * FNV1A_PRIME;

            found = find_key( hash );
            if( found < 0 )
                insert_key( hash, -found );

            while( *p == LF )
                p++;
        } while( *p );
    }

    free( buf );

    printf( "= Mem =\n" );
    printf( "  File buffer: %lu\n", (unsigned long) size+1 );
    printf( "  Hash buffer: %lu\n", (unsigned long) sizeof( gaWords ) );
    printf( "  ===========: %lu\n", (unsigned long) size+1 + sizeof( gaWords ) );
    printf( "%ld unique lines\n", gnWords );

    return 0;
}

