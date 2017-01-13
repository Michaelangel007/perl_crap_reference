/*
https://github.com/Michaelangel007/perl_crap_reference

Test 2 - Version 2 Optimized OpenMP

* Read entire file into memory
* OpenMP

*/

    #include <stdio.h>    // printf()
    #include <string.h>   // memmove()
    #include <stdint.h>   // uint32_t
    #include <stdlib.h>   // malloc()
    #include <sys/stat.h> // stat()

    #include "text_itoa.h"// itoa()
    #include "inline.h"   // INLINE

// BEGIN OMP
    #include <omp.h>

    #define MAX_THREADS 8

    int gnThreadsActive  = 0;
    int gnThreadsMaximum = 0;

    size_t   ganThreadLines[ MAX_THREADS ]; // Sub-total lines per thread
    unsigned ganThreadBeg  [ MAX_THREADS ]; // File Offset Begin per thread
    unsigned ganThreadEnd  [ MAX_THREADS ]; // File Offset End   per thread
// END OMP

    #define LF  0x0A

    size_t gnLines = 0;


// ========================================================================
int main( const int nArg, const char *aArg[] )
{
    char *filename = "words.txt";

// BEGIN OMP
    int iThread;

    unsigned int beg;
    unsigned int end;

    size_t nThreadDataElem = 0;

    int iArg = 0;

    gnThreadsMaximum = omp_get_num_procs();

    if( nArg > 1 )
    {
        while( iArg < nArg )
        {
            char *pArg = (char*) aArg[ iArg + 1 ];
            if(  !pArg )
                break;

            if( pArg[0] == '-' )
            {
                iArg++;
                pArg++; // point to 1st char in option

                if( *pArg == 'j' )
                {
                    int i = atoi( pArg+1 );
                    if( i > 0 )
                        gnThreadsActive = i;
                }
            }
            else
                filename = pArg;

            iArg++;
        }
    }

    if(!gnThreadsActive) // user didn't specify how many threads to use, default to all of them
        gnThreadsActive = gnThreadsMaximum;

    omp_set_num_threads( gnThreadsActive );
    printf( "Using: %d / %d cores\n", gnThreadsActive, gnThreadsMaximum );

// END OMP

    struct stat info;

    FILE  *data = fopen( filename, "rb" );
    size_t size = stat( filename, &info ) ? 0 : (size_t) info.st_size;

    unsigned char *buf = (unsigned char*) malloc( size+1 );

    if( !data )
        return printf( "ERROR: Couldn't open input file: %s\n", filename );

    if( !buf )
        return printf( "ERROR: Couldn't allocate memory for file\n" );

    fread( buf, size, 1, data );
    buf[ size ] = 0;

    fclose( data );

// BEGIN OMP

    // If we have less data then threads some threads will have nothing to do
    if( size < gnThreadsActive )
    {
        printf( "INFO: File size less then number of Threads. Defaulting to 1 thread.\n" );
        gnThreadsActive = 1;
    }

    nThreadDataElem = size / gnThreadsActive;

    memset( ganThreadBeg, 0, sizeof( ganThreadBeg ) );
    memset( ganThreadEnd, 0, sizeof( ganThreadEnd ) );

    end = 0;
    for( iThread = 0; iThread < gnThreadsActive; iThread++ )
    {
        beg = end;
        end = beg + nThreadDataElem - 1;

        end++;

        if( end > size ) end = size;

        ganThreadLines[ iThread ] = 0;

        ganThreadBeg[ iThread ] = beg;
        ganThreadEnd[ iThread ] = end;
    }
// END OMP

    // Force very last segment to end file size
    ganThreadEnd[ gnThreadsActive-1 ] = size;

// Scatter ________________________________________________________________

// BEGIN OMP
#pragma omp parallel for
    for( iThread = 0; iThread < gnThreadsActive; ++iThread )
    {
        const int tid = omp_get_thread_num(); // Get Thread Index: 0 .. nCores-1

        unsigned int b = ganThreadBeg[ tid ];
        unsigned int e = ganThreadEnd[ tid ];

        // If not enough work to do for this thread, skip
        if( b >= e ) // Thread Idle
            continue;
// END OMP

        do
        {
            while( (b < e) && buf[ b ] != LF )
                b++;

            while( (b < e) && buf[ b ] == LF )
            {
                ganThreadLines[ tid ]++;
                b++;
            }

        } while( b < e );
    }

// BEGIN OMP

// Gather _________________________________________________________________

    for( iThread = 0; iThread <  gnThreadsActive; ++iThread )
        gnLines += ganThreadLines[ iThread ];

    free( buf );

    printf( "%s lines\n", itoaComma( gnLines, NULL ) );

    return 0;
}

