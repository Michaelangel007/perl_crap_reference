/*
https://github.com/Michaelangel007/perl_crap_reference

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

    #define SPC 0x20

    #define MAX_WORDS  80000

    size_t   gnWords = 0;
    uint32_t gaWords[ MAX_WORDS ]; // 320,000 bytes

// ========================================================================
INLINE int find_key( uint32_t key )
{
    int i;

    for( i = 0; i < gnWords; ++i )
        if( gaWords[ i ] == key )
            return i+1;

    return 0;
}

// ========================================================================
INLINE void add_key( uint32_t key )
{
    gaWords[ gnWords++ ] = key;
}

// ========================================================================
int main()
{
    const char *filename = "words.txt";
    struct stat info;

    FILE  *data = fopen( filename, "rb" );
    size_t size = stat( filename, &info ) ? 0 : (size_t) info.st_size;

    char    *buf = (char*) malloc( size+1 );
    uint32_t hash;

    if( data )
    {
        fread( buf, size, 1, data );
        buf[ size ] = 0;
        fclose( data );

        char *p = buf;

        do
        {
            hash = FNV1A_SEED;
            while( *p > SPC )
                hash =  (*p++ ^ hash) * FNV1A_PRIME;

            if( !find_key( hash ) )
                add_key( hash );

            p++; // skip LF = 0x0A
        } while( *p );
    }

    printf( "= Mem =\n" );
    printf( "  File buffer: %lu\n", (unsigned long) size+1                     );
    printf( "  Hash buffer: %lu\n", (unsigned long)          sizeof( gaWords ) );
    printf( "  ===========: %lu\n", (unsigned long) size+1 + sizeof( gaWords ) );
    printf( "%ld unique lines\n", gnWords );

    return 0;
}

