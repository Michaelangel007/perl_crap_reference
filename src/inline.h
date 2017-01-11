/*
Force inlining for MSVC and GCC
*/
#ifdef __MSVC
    #define INLINE __forceinline
#else
    #define INLINE __attribute__((always_inline))
#endif
