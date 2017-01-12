    #include <stdio.h>
    #include <string.h>

    #define MAX_WORDS  80000 // Increased from 30,000 to 80,000
    #define MAX_STRING 255

    long gnWords = 0;
    char gaWords[ MAX_WORDS ][ MAX_STRING+1 ]; // 2,0480,000 bytes

// ========================================================================
void AddToList( char *word )
{
    long i;

    for( i = 0; i < gnWords; i++ )
    {
        if( strcmp( word, gaWords[i]) == 0)
            return;
    }

    strcpy( gaWords[ gnWords++ ], word ); // Fixed off-by-one bug
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
    printf( "  File buffer: %lu\n", (unsigned long) sizeof( line )                     );
    printf( "  Hash buffer: %lu\n", (unsigned long)                  sizeof( gaWords ) );
    printf( "  ===========: %lu\n", (unsigned long) sizeof( line ) + sizeof( gaWords ) );
    printf( "%ld unique lines\n", gnWords );

    return 0;
}

