/*
https://github.com/Michaelangel007/perl_crap_reference

Optimized 6

* Read entire file into memory
* Inlined FNV1A
* Inlined BinarySearchInsert
* OpenMP

*/

    #include <stdio.h>    // printf()
    #include <string.h>   // memmove()
    #include <stdint.h>   // uint32_t
    #include <stdlib.h>   // malloc()
    #include <sys/stat.h> // stat()
    #include "inline.h"   // INLINE

// BEGIN OMP
    #include <omp.h>

    #define MAX_THREADS 8

    int gnThreadsActive  = 0;
    int gnThreadsMaximum = 0;

    uint32_t *gapThreadData[ MAX_THREADS ]; // array of hashes        per thread
    size_t    ganThreadWord[ MAX_THREADS ]; // Sub-total unique lines per thread

    unsigned ganOffsetBeg[ MAX_THREADS ]; // File Offset Begin per thread
    unsigned ganOffsetEnd[ MAX_THREADS ]; // File Offset End   per thread
// END OMP

    const uint32_t FNV1A_PRIME = 0x01000193; //   16777619
    const uint32_t FNV1A_SEED  = 0x811C9DC5; // 2166136261

    #define LF  0x0A
    #define SPC 0x20

    size_t gnWords = 0;

// If the key is  found, returns position > 0 where key was found
// If the key not found, returns -1 but with key inserted
// ========================================================================
INLINE void find_key_insert( const int iThread,  uint32_t key )
{
    uint32_t *data = gapThreadData[ iThread ];
    size_t    size = ganThreadWord[ iThread ];

    int min = 0;
    int mid = 0;
    int max = size - 1;

    while( min <= max )
    {
        mid = (min + max) >> 1;

        /**/ if( data[ mid ] == key )   return;
        else if( data[ mid ] >  key )   max  = mid-1;
        else /*              <  key )*/ min  = mid+1;
    }

    while( (mid < size) && (key > data[ mid ]) )
        mid++;

    /* */ uint32_t *src = &data[ mid ];
    /* */ uint32_t *dst = src + 1;
    const size_t    len = size - mid;

    memmove( dst, src, sizeof( uint32_t ) * len ); // memcpy() can't alias (overlap)
    data[ mid ] = key;

    ganThreadWord[ iThread ]++;
}

// ========================================================================
int main( const int nArg, const char *aArg[] )
{
    const char *filename = "words.txt";

// BEGIN OMP
    int iThread;

    unsigned int beg;
    unsigned int end;

    size_t nThreadDataElem = 0;
    size_t nThreadDataSize = 0;

    int iArg = 0;

    gnThreadsMaximum = omp_get_num_procs();

    if( nArg > 1 )
    {
        while( iArg < nArg )
        {
            iArg++;

            char *pArg = (char*) aArg[ iArg ];
            if(  !pArg )
                break;

            if( pArg[0] == '-' )
            {
                pArg++; // point to 1st char in option

                if( *pArg == 'j' )
                {
                    int i = atoi( pArg+1 );
                    if( i > 0 )
                        gnThreadsActive = i;
                }
            }
            else
                filename = aArg[ iArg ];
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

    unsigned char *buf = (unsigned char*) malloc( size+2 );

    if( !data )
        return printf( "ERROR: Couldn't open input file: %s\n", filename );

    if( !buf )
        return printf( "ERROR: Couldn't allocate memory for file\n" );

    fread( buf, size, 1, data );
    buf[ size+0 ] = LF;
    buf[ size+1 ] = 0;

    fclose( data );

// BEGIN OMP

    // If we have less data then threads some threads will have nothing to do
    if( size < gnThreadsActive )
    {
        printf( "INFO: File size less then number of Threads. Defaulting to 1 thread.\n" );
        gnThreadsActive = 1;
    }

    nThreadDataElem = size / gnThreadsActive;
    nThreadDataSize = sizeof( uint32_t ) * nThreadDataElem;

    memset( ganOffsetBeg, 0, sizeof( ganOffsetBeg ) );
    memset( ganOffsetEnd, 0, sizeof( ganOffsetEnd ) );

    end = 0;
    for( iThread = 0; iThread < gnThreadsActive; iThread++ )
    {
        beg = end;
        end = beg + nThreadDataElem - 1;

        if( beg > size) beg = size;
        if( end > size) end = size-1;

        gapThreadData[ iThread ] = (uint32_t*) malloc( nThreadDataSize );
        ganThreadWord[ iThread ] = 0;

        // Adjust end to next LF
        while( buf[ end ] != LF ) // TODO: check for buffer overflow?
            end++;

        end++;

        if( !ganOffsetBeg[ iThread ] ) ganOffsetBeg[ iThread ] = beg;
        if( !ganOffsetEnd[ iThread ] ) ganOffsetEnd[ iThread ] = end;
    }
// END OMP

    // Force very last segment to end file size
    ganOffsetEnd[ gnThreadsActive-1 ] = size;

// Scatter ________________________________________________________________

// BEGIN OMP
#pragma omp parallel for
    for( iThread = 0; iThread < gnThreadsActive; ++iThread )
    {
        const int id = omp_get_thread_num(); // Get Thread Index: 0 .. nCores-1
        uint32_t hash;

        unsigned int b = ganOffsetBeg[ id ];
        unsigned int e = ganOffsetEnd[ id ];

        // If not enough work to do for this thread, skip
        if( b >= e ) // Thread Idle
            continue;
// END OMP

        do
        {
            hash = FNV1A_SEED;

            while( buf[ b ] != LF ) // MSDOG: 0xD 0xA
                hash = (buf[ b++ ] ^ hash) * FNV1A_PRIME;

            find_key_insert( id, hash );

            while( buf[ b ] == LF )
                b++;
        } while( b < e );
    }

// BEGIN OMP

// Gather _________________________________________________________________

    int nLeft   =  0; // Maximum possible # of hashes left to sort
    int iMinCol = -1; // which thread has current lowest hash
    int nMinVal =  0;
    int _anThreadHead[ MAX_THREADS ]; // # of hashes been processed for each thread

    for( iThread = 0; iThread <  gnThreadsActive; ++iThread )
    {
        nLeft += ganThreadWord[ iThread ];
        _anThreadHead[ iThread ] = 0;
    }

    int i, n;
    uint32_t hash;

    // n-way MergeSort into 1 unique column
    // We don't actually need to store the final array
    // as we only need to keep track of the lowest current hash across all columns
    while( nLeft > 0 )
    {
        iMinCol = -1;

        for( iThread = 0; iThread <  gnThreadsActive; ++iThread )
        {
            i = _anThreadHead[ iThread ];
            n = ganThreadWord[ iThread ];

            // If this column has not finished merging ...
            if( i < n )
            {
                hash = gapThreadData[ iThread ][ i ];

                // Look for new lowest candidate
                if( iMinCol < 0 )
                {
                    iMinCol = iThread;
                    nMinVal = hash;
                }
                else
                    if( nMinVal > hash )
                    {
                        nMinVal = hash ;
                        iMinCol = iThread;
                    }
            }
        }

        for( iThread = 0; iThread <  gnThreadsActive; ++iThread )
        {
            if( iMinCol != iThread )
            {
                i = _anThreadHead[ iThread ];
                n = ganThreadWord[ iThread ];

                if( i < n )
                {
                    hash = gapThreadData[ iThread ][ i ];

                    if( nMinVal == hash ) // remove duplicate if not in min col
                    {
                        _anThreadHead[ iThread ]++;
                        nLeft--;
                    }
                }
            }
        }

        if( iMinCol >= 0)
        {
            _anThreadHead[ iMinCol ]++;
            gnWords++;

            nLeft--;
        }
    }

    free( buf );

    for( iThread = 0; iThread < gnThreadsActive; iThread++ )
        free( gapThreadData[ iThread ] );

    printf( "= Mem =\n" );
    printf( "  File buffer: %lu\n", (unsigned long) size                   );
    printf( "  Hash buffer: %lu  ", (unsigned long)        nThreadDataSize );
    printf( "* %d = %lu\n"        , gnThreadsActive,       nThreadDataSize*gnThreadsActive );
    printf( "  ===========: %lu\n", (unsigned long)(size + nThreadDataSize*gnThreadsActive));

    printf( "%ld unique lines\n", gnWords );

    return 0;
}

