#pragma once

#if defined(_WIN32)
#define CMAKE_SHARED_LIBRARY_PREFIX ""
#define CMAKE_SHARED_LIBRARY_SUFFIX ".dll"
#elif defined(__APPLE__)
#define CMAKE_SHARED_LIBRARY_PREFIX "lib"
#define CMAKE_SHARED_LIBRARY_SUFFIX ".dylib"
#else
#define CMAKE_SHARED_LIBRARY_PREFIX "lib"
#define CMAKE_SHARED_LIBRARY_SUFFIX ".so"
#endif
