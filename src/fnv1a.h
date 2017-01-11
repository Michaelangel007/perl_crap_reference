/*

Reference:

* http://create.stephan-brumme.com/fnv-hash/

*/

const uint32_t FNV1A_PRIME = 0x01000193; //   16777619
const uint32_t FNV1A_SEED  = 0x811C9DC5; // 2166136261

// C++ header
//INLINE uint32_t fnv1a_byte( unsigned char byte, uint32_t hash = FNV1A_SEED )
// C header
INLINE uint32_t fnv1a_byte( unsigned char byte, uint32_t hash )
{
    return (byte ^ hash) * FNV1A_PRIME;
}

// C++ header
//INLINE uint32_t fnv1a_data( size_t len, void *data, uint32_t hash = FNV1A_SEED )
// C header
INLINE uint32_t fnv1a_data( size_t len, void *data )
{
    uint32_t       hash = FNV1A_SEED;
    unsigned char *p    = (unsigned char *)data;
    while( len --> 0 )
        hash = fnv1a_byte( *p++, hash );

    return hash;
}

// C++ header
//INLINE uint32_t fnv1a_string( char *text, uint32_t hash = FNV1A_SEED )
// C header
INLINE uint32_t fnv1a_string( char *text )
{
    uint32_t       hash = FNV1A_SEED;
    unsigned char *p    = (unsigned char *)text;

    while( *p )
        hash = fnv1a_byte( *p++, hash );

    return hash;
}

