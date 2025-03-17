#include <stdio.h>

#if defined( __clang__)
    #define COMPILER "clang"
#elif defined (__TINYC__ )
    #define COMPILER "TCC"
#elif defined (__GNUC__ )
    #define COMPILER "GCC"
#elif defined (_MSC_VER)
    #define COMPILER "MSVC"
#endif

#ifdef NDEBUG
    #define ASSERTS "disabled"
#else
    #define ASSERTS "enabled"
#endif



int main() {

    printf("Compiler: %s\n", COMPILER);
    printf("Bitness: %d\n", ((int) sizeof (long) == 8) ? 64 : 32);
    printf("Asserts: %s\n", ASSERTS);

    return 0;
}