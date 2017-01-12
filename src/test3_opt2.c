/*

Optimize 2

* Use FNV1A Hash

*/

    #include <stdio.h>    // printf()
    #include <stdint.h>   // uint32_t

    #include "inline.h"   // INLINE
    #include "fnv1a.h"    // fnva1_string()

    #define MAX_WORDS  80000
    #define MAX_STRING 255

    size_t   gnWords = 0;
    uint32_t gaWords[ MAX_WORDS ]; // 320,000 bytes

// ========================================================================
void AddToList( char *word )
{
    size_t i;

    uint32_t hash = fnv1a_string( word );

    for( i = 0; i < gnWords; i++ )
    {
        if( hash == gaWords[i] )
            return;
    }

    gaWords[ gnWords++ ] = hash;
}

// ========================================================================
int main()
{
    char line[ MAX_STRING+1 ];
    FILE *data = fopen( "words.txt", "r" );

    if( data )
    {
        while( !feof( data ) )
        {
            fgets( line, MAX_STRING, data );
            AddToList( line );
        }
    }

    fclose( data );

    printf( "= Mem =\n" );
    printf( "  File buffer: %lu\n", (unsigned long) sizeof( line ) );
    printf( "  Hash buffer: %lu\n", (unsigned long)                  sizeof( gaWords ) );
    printf( "  ===========: %lu\n", (unsigned long) sizeof( line ) + sizeof( gaWords ) );
    printf( "%ld unique lines\n", gnWords );

    return 0;
}

