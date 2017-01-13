/**
 @param  {uint64_t}  x        - Number to convert to comma delimited string
 @param  {char*}    [output_] - Optional buffer to copy output to
 @return {char*} Returns pointer to comma delimited NULL terminated string
 @notes          Does NOT NULL terminated the output string
 @history
    Version 8 - Plain C version
    Version 7 - cleanup 'x > 9' to 'x > 0'
    Version 6 - Add jsdoc
    Verison 5 - add optional output, does NOT null terminate output
    Version 4 - document hard-coded max string length 32
    Version 3 - cleanup '>= 10' to '> 9'
    Verison 2 - add 4 buffers
    Version 1 - initial implementation
*/
const char*
itoaComma( uint64_t x, char *output_ /* = NULL */ )
{
    #define MAX_CHARS   31 // 2^32 - 1 =              4,294,967,295 = 13 chars
    #define NUM_BUFFERS  4 // 2^64 - 1 = 18,446,744,073,709,551,615 = 26 chars

    static char    aString[ NUM_BUFFERS ][ MAX_CHARS+1 ];
    static uint8_t iString     = 0;

    char *p = &aString[ iString ][ MAX_CHARS ];
    char *e = p; // save copy of end pointer

    *p-- = 0;
    while( x > 999 )
    {
        *p-- = '0' + (x % 10); x /= 10;
        *p-- = '0' + (x % 10); x /= 10;
        *p-- = '0' + (x % 10); x /= 10;
        *p-- = ',';
    }

    /*      */ { *p-- = '0' + (x % 10); x /= 10; }
    if (x > 0) { *p-- = '0' + (x % 10); x /= 10; }
    if (x > 0) { *p-- = '0' + (x % 10); x /= 10; }

    iString++;
    iString &= (NUM_BUFFERS-1);

    if( output_ )
        memcpy( output_, p+1, e-p-1 ); // -1 to NOT include NULL terminator

    return ++p;

    #undef MAX_CHARS
    #undef NUM_BUFFERS
}
