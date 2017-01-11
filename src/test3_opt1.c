#include <stdio.h>
#include <string.h>

#define MAX_WORDS  80000
#define MAX_STRING 255

char gaWords[ MAX_WORDS ][ MAX_STRING+1 ]; // 2,0480,000 bytes
long gnWords = 0;

void AddToList( char *word )
{
    long i;

    for( i = 0; i < gnWords; i++ )
    {
        if( strcmp( word, gaWords[i]) == 0)
            return;
    }

    strcpy( gaWords[ gnWords++ ], word );
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

