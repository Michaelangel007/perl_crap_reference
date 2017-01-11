/*

Optimize 2

* Use FNV1A Hash

*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "inline.h"
#include "fnv1a.h"

#define MAX_WORDS  80000
#define MAX_STRING 255

uint32_t gaWords[ MAX_WORDS ]; // 320,000 bytes
size_t   gnWords = 0;

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

int main()
{
    char line[ MAX_STRING+1 ];
    FILE *data;

    data = fopen( "words.txt", "r" );

    while( !feof( data ) )
    {
        fgets( line, MAX_STRING, data );
        AddToList( line );
    }

//  printf( "Allocated: %lu bytes\n", (unsigned long) sizeof( gaWords ) );
    printf( "%ld unique lines\n", gnWords );

    return 0;
}

