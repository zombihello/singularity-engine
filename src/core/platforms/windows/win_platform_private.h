#ifndef WIN_PLATFORM_PRIVATE_H
#define WIN_PLATFORM_PRIVATE_H

// Windows support _aligned_malloc
#undef PLATFORM_USE__ALIGNED_MALLOC
#define PLATFORM_USE__ALIGNED_MALLOC                1

// On Windows ANSI malloc is thread safe
#undef PLATFORM_IS_ANSI_MALLOC_THREADSAFE
#define PLATFORM_IS_ANSI_MALLOC_THREADSAFE          1

// If we on 64 bit platform then it is supports mimalloc
#undef PLATFORM_SUPPORTS_MIMALLOC
#define PLATFORM_SUPPORTS_MIMALLOC                  PLATFORM_64BIT

#endif // !WIN_PLATFORM_PRIVATE_H