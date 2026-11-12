#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_H 1
#define BUG_REPORT_URL "http://llvm.org/bugs/"
#if defined(__GLIBC__)
#define ENABLE_BACKTRACES 1
#endif
#define ENABLE_CRASH_OVERRIDES 1
#define DISABLE_LLVM_DYLIB_ATEXIT 1
#define ENABLE_PIC 1
#define ENABLE_TIMESTAMPS 1
#if defined(_WIN32)
/* #undef HAVE_DECL_ARC4RANDOM */
#elif defined(__APPLE__)
#define HAVE_DECL_ARC4RANDOM 1
#else
#if defined(__GLIBC__) && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 36))
#define HAVE_DECL_ARC4RANDOM 1
#endif
#endif
#if defined(_WIN32)
/* #undef HAVE_BACKTRACE */
#elif defined(__APPLE__)
#define HAVE_BACKTRACE 1
#else
#if defined(__GLIBC__)
#define HAVE_BACKTRACE 1
#endif
#endif
#if defined(_WIN32)
/* #undef HAVE_CLOSEDIR */
#elif defined(__APPLE__)
#define HAVE_CLOSEDIR 1
#else
#define HAVE_CLOSEDIR 1
#endif
#if defined(_WIN32)
/* #undef HAVE_CXXABI_H */
#elif defined(__APPLE__)
#define HAVE_CXXABI_H 1
#else
#define HAVE_CXXABI_H 1
#endif
#if defined(_WIN32)
#define HAVE_DECL_STRERROR_S 1
#elif defined(__APPLE__)
#define HAVE_DECL_STRERROR_S 0
#else
#define HAVE_DECL_STRERROR_S 0
#endif
/* #undef HAVE_DIA_SDK */
#if defined(_WIN32)
/* #undef HAVE_DIRENT_H */
#elif defined(__APPLE__)
#define HAVE_DIRENT_H 1
#else
#define HAVE_DIRENT_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_DLERROR */
#elif defined(__APPLE__)
#define HAVE_DLERROR 1
#else
#define HAVE_DLERROR 1
#endif
#if defined(_WIN32)
/* #undef HAVE_DLFCN_H */
#elif defined(__APPLE__)
#define HAVE_DLFCN_H 1
#else
#define HAVE_DLFCN_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_DLOPEN */
#elif defined(__APPLE__)
#define HAVE_DLOPEN 1
#else
#define HAVE_DLOPEN 1
#endif
#define HAVE_ERRNO_H 1
#if defined(_WIN32)
/* #undef HAVE_EXECINFO_H */
#elif defined(__APPLE__)
#define HAVE_EXECINFO_H 1
#else
#if defined(__GLIBC__)
#define HAVE_EXECINFO_H 1
#endif
#endif
#define HAVE_FCNTL_H 1
#define HAVE_FENV_H 1
/* #undef HAVE_FFI_CALL */
/* #undef HAVE_FFI_FFI_H */
/* #undef HAVE_FFI_H */
#if defined(_WIN32)
/* #undef HAVE_FUTIMES */
#elif defined(__APPLE__)
#define HAVE_FUTIMES 1
#else
#define HAVE_FUTIMES 1
#endif
#if defined(_WIN32)
/* #undef HAVE_FUTIMENS */
#elif defined(__APPLE__)
#define HAVE_FUTIMENS 1
#else
#define HAVE_FUTIMENS 1
#endif
#if defined(_WIN32)
/* #undef HAVE_GETCWD */
#elif defined(__APPLE__)
#define HAVE_GETCWD 1
#else
#define HAVE_GETCWD 1
#endif
#if defined(_WIN32)
/* #undef HAVE_GETPAGESIZE */
#elif defined(__APPLE__)
#define HAVE_GETPAGESIZE 1
#else
#define HAVE_GETPAGESIZE 1
#endif
#if defined(_WIN32)
/* #undef HAVE_GETRLIMIT */
#elif defined(__APPLE__)
#define HAVE_GETRLIMIT 1
#else
#define HAVE_GETRLIMIT 1
#endif
#if defined(_WIN32)
/* #undef HAVE_GETRUSAGE */
#elif defined(__APPLE__)
#define HAVE_GETRUSAGE 1
#else
#define HAVE_GETRUSAGE 1
#endif
#if defined(_WIN32)
/* #undef HAVE_GETTIMEOFDAY */
#elif defined(__APPLE__)
#define HAVE_GETTIMEOFDAY 1
#else
#define HAVE_GETTIMEOFDAY 1
#endif
#define HAVE_INT64_T 1
#define HAVE_INTTYPES_H 1
#if defined(_WIN32)
/* #undef HAVE_ISATTY */
#elif defined(__APPLE__)
#define HAVE_ISATTY 1
#else
#define HAVE_ISATTY 1
#endif
#if defined(_WIN32)
/* #undef HAVE_LIBDL */
#elif defined(__APPLE__)
#define HAVE_LIBDL 1
#else
#define HAVE_LIBDL 1
#endif
/* #undef HAVE_LIBPSAPI */
#if defined(_WIN32)
/* #undef HAVE_LIBPTHREAD */
#elif defined(__APPLE__)
#define HAVE_LIBPTHREAD 1
#else
#define HAVE_LIBPTHREAD 1
#endif
/* #undef HAVE_LIBSHELL32 */
/* #undef HAVE_LIBZ */
/* #undef HAVE_LIBEDIT */
#define HAVE_LIMITS_H 1
#if defined(_WIN32)
/* #undef HAVE_LINK_H */
#elif defined(__APPLE__)
/* #undef HAVE_LINK_H */
#else
#define HAVE_LINK_H 1
#endif
#define HAVE_LINK_EXPORT_DYNAMIC 1
/* #undef HAVE_LONGJMP */
#if defined(_WIN32)
/* #undef HAVE_MACH_MACH_H */
#elif defined(__APPLE__)
#define HAVE_MACH_MACH_H 1
#else
/* #undef HAVE_MACH_MACH_H */
#endif
#if defined(_WIN32)
/* #undef HAVE_MACH_O_DYLD_H */
#elif defined(__APPLE__)
#define HAVE_MACH_O_DYLD_H 1
#else
/* #undef HAVE_MACH_O_DYLD_H */
#endif
#if defined(_WIN32)
/* #undef HAVE_MALLINFO */
#elif defined(__APPLE__)
/* #undef HAVE_MALLINFO */
#else
#if defined(__GLIBC__)
#define HAVE_MALLINFO 1
#endif
#endif
#if defined(_WIN32)
/* #undef HAVE_MALLINFO2 */
#elif defined(__APPLE__)
/* #undef HAVE_MALLINFO2 */
#else
#if defined(__GLIBC__) && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 33))
#define HAVE_MALLINFO2 1
#endif
#endif
#if defined(_WIN32)
#define HAVE_MALLOC_H 1
#elif defined(__APPLE__)
/* #undef HAVE_MALLOC_H */
#else
#define HAVE_MALLOC_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_MALLOC_MALLOC_H */
#elif defined(__APPLE__)
#define HAVE_MALLOC_MALLOC_H 1
#else
/* #undef HAVE_MALLOC_MALLOC_H */
#endif
#if defined(_WIN32)
/* #undef HAVE_MALLOC_ZONE_STATISTICS */
#elif defined(__APPLE__)
#define HAVE_MALLOC_ZONE_STATISTICS 1
#else
/* #undef HAVE_MALLOC_ZONE_STATISTICS */
#endif
/* #undef HAVE_MALLCTL */
#if defined(_WIN32)
/* #undef HAVE_MKDTEMP */
#elif defined(__APPLE__)
#define HAVE_MKDTEMP 1
#else
#define HAVE_MKDTEMP 1
#endif
#if defined(_WIN32)
/* #undef HAVE_MKSTEMP */
#elif defined(__APPLE__)
#define HAVE_MKSTEMP 1
#else
#define HAVE_MKSTEMP 1
#endif
#if defined(_WIN32)
/* #undef HAVE_MKTEMP */
#elif defined(__APPLE__)
#define HAVE_MKTEMP 1
#else
#define HAVE_MKTEMP 1
#endif
/* #undef HAVE_NDIR_H */
#if defined(_WIN32)
/* #undef HAVE_OPENDIR */
#elif defined(__APPLE__)
#define HAVE_OPENDIR 1
#else
#define HAVE_OPENDIR 1
#endif
#if defined(_WIN32)
/* #undef HAVE_POSIX_SPAWN */
#elif defined(__APPLE__)
#define HAVE_POSIX_SPAWN 1
#else
#define HAVE_POSIX_SPAWN 1
#endif
#if defined(_WIN32)
/* #undef HAVE_PREAD */
#elif defined(__APPLE__)
#define HAVE_PREAD 1
#else
#define HAVE_PREAD 1
#endif
#if defined(_WIN32)
/* #undef HAVE_PTHREAD_GETSPECIFIC */
#elif defined(__APPLE__)
#define HAVE_PTHREAD_GETSPECIFIC 1
#else
#define HAVE_PTHREAD_GETSPECIFIC 1
#endif
#if defined(_WIN32)
/* #undef HAVE_PTHREAD_H */
#elif defined(__APPLE__)
#define HAVE_PTHREAD_H 1
#else
#define HAVE_PTHREAD_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_PTHREAD_MUTEX_LOCK */
#elif defined(__APPLE__)
#define HAVE_PTHREAD_MUTEX_LOCK 1
#else
#define HAVE_PTHREAD_MUTEX_LOCK 1
#endif
#if defined(_WIN32)
/* #undef HAVE_PTHREAD_RWLOCK_INIT */
#elif defined(__APPLE__)
#define HAVE_PTHREAD_RWLOCK_INIT 1
#else
#define HAVE_PTHREAD_RWLOCK_INIT 1
#endif
#if defined(_WIN32)
/* #undef HAVE_RAND48 */
#elif defined(__APPLE__)
#define HAVE_RAND48 1
#else
#define HAVE_RAND48 1
#endif
#if defined(_WIN32)
/* #undef HAVE_READDIR */
#elif defined(__APPLE__)
#define HAVE_READDIR 1
#else
#define HAVE_READDIR 1
#endif
#if defined(_WIN32)
/* #undef HAVE_REALPATH */
#elif defined(__APPLE__)
#define HAVE_REALPATH 1
#else
#define HAVE_REALPATH 1
#endif
#if defined(_WIN32)
/* #undef HAVE_SBRK */
#elif defined(__APPLE__)
#define HAVE_SBRK 1
#else
#define HAVE_SBRK 1
#endif
#if defined(_WIN32)
/* #undef HAVE_SETENV */
#elif defined(__APPLE__)
#define HAVE_SETENV 1
#else
#define HAVE_SETENV 1
#endif
/* #undef HAVE_SETJMP */
#if defined(_WIN32)
/* #undef HAVE_SETRLIMIT */
#elif defined(__APPLE__)
#define HAVE_SETRLIMIT 1
#else
#define HAVE_SETRLIMIT 1
#endif
/* #undef HAVE_SIGLONGJMP */
#define HAVE_SIGNAL_H 1
/* #undef HAVE_SIGSETJMP */
#define HAVE_STDINT_H 1
/* #undef HAVE_STRDUP */
#define HAVE_STRERROR 1
#if defined(_WIN32)
/* #undef HAVE_STRERROR_R */
#elif defined(__APPLE__)
#define HAVE_STRERROR_R 1
#else
#define HAVE_STRERROR_R 1
#endif
#define HAVE_STRTOLL 1
#if defined(_WIN32)
/* #undef HAVE_STRTOQ */
#elif defined(__APPLE__)
#define HAVE_STRTOQ 1
#else
#define HAVE_STRTOQ 1
#endif
#if defined(_WIN32)
/* #undef HAVE_SYS_DIR_H */
#elif defined(__APPLE__)
/* #undef HAVE_SYS_DIR_H */
#else
#define HAVE_SYS_DIR_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_SYS_IOCTL_H */
#elif defined(__APPLE__)
#define HAVE_SYS_IOCTL_H 1
#else
#define HAVE_SYS_IOCTL_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_SYS_MMAN_H */
#elif defined(__APPLE__)
#define HAVE_SYS_MMAN_H 1
#else
#define HAVE_SYS_MMAN_H 1
#endif
/* #undef HAVE_SYS_NDIR_H */
#if defined(_WIN32)
/* #undef HAVE_SYS_PARAM_H */
#elif defined(__APPLE__)
#define HAVE_SYS_PARAM_H 1
#else
#define HAVE_SYS_PARAM_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_SYS_RESOURCE_H */
#elif defined(__APPLE__)
#define HAVE_SYS_RESOURCE_H 1
#else
#define HAVE_SYS_RESOURCE_H 1
#endif
#define HAVE_SYS_STAT_H 1
#if defined(_WIN32)
/* #undef HAVE_SYS_TIME_H */
#elif defined(__APPLE__)
#define HAVE_SYS_TIME_H 1
#else
#define HAVE_SYS_TIME_H 1
#endif
/* #undef HAVE_SYS_TYPES_H */
#if defined(_WIN32)
/* #undef HAVE_SYS_UIO_H */
#elif defined(__APPLE__)
#define HAVE_SYS_UIO_H 1
#else
#define HAVE_SYS_UIO_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_SYS_WAIT_H */
#elif defined(__APPLE__)
#define HAVE_SYS_WAIT_H 1
#else
#define HAVE_SYS_WAIT_H 1
#endif
/* #undef HAVE_TERMINFO */
#if defined(_WIN32)
/* #undef HAVE_TERMIOS_H */
#elif defined(__APPLE__)
#define HAVE_TERMIOS_H 1
#else
#define HAVE_TERMIOS_H 1
#endif
#define HAVE_UINT64_T 1
#if defined(_WIN32)
/* #undef HAVE_UNISTD_H */
#elif defined(__APPLE__)
#define HAVE_UNISTD_H 1
#else
#define HAVE_UNISTD_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_UTIME_H */
#elif defined(__APPLE__)
#define HAVE_UTIME_H 1
#else
#define HAVE_UTIME_H 1
#endif
#if defined(_WIN32)
/* #undef HAVE_U_INT64_T */
#elif defined(__APPLE__)
#define HAVE_U_INT64_T 1
#else
#define HAVE_U_INT64_T 1
#endif
/* #undef HAVE_VALGRIND_VALGRIND_H */
#if defined(_WIN32)
/* #undef HAVE_WRITEV */
#elif defined(__APPLE__)
#define HAVE_WRITEV 1
#else
#define HAVE_WRITEV 1
#endif
#if defined(_WIN32)
/* #undef HAVE_ZLIB_H */
#elif defined(__APPLE__)
#define HAVE_ZLIB_H 1
#else
#define HAVE_ZLIB_H 1
#endif
/* #undef HAVE__ALLOCA */
/* #undef HAVE___ALLOCA */
/* #undef HAVE___ASHLDI3 */
/* #undef HAVE___ASHRDI3 */
#if defined(_WIN32)
#define HAVE___CHKSTK 1
#elif defined(__APPLE__)
/* #undef HAVE___CHKSTK */
#else
/* #undef HAVE___CHKSTK */
#endif
/* #undef HAVE___CHKSTK_MS */
/* #undef HAVE___CMPDI2 */
/* #undef HAVE___DIVDI3 */
/* #undef HAVE___FIXDFDI */
/* #undef HAVE___FIXSFDI */
/* #undef HAVE___FLOATDIDF */
/* #undef HAVE___LSHRDI3 */
/* #undef HAVE___MAIN */
/* #undef HAVE___MODDI3 */
/* #undef HAVE___UDIVDI3 */
/* #undef HAVE___UMODDI3 */
/* #undef HAVE____CHKSTK */
/* #undef HAVE____CHKSTK_MS */
/* #undef LLVM_BINDIR */
/* #undef LLVM_CONFIGTIME */
/* #undef LLVM_DATADIR */
#define LLVM_DEFAULT_TARGET_TRIPLE "dxil-ms-dx"
/* #undef LLVM_DOCSDIR */
#define LLVM_ENABLE_THREADS 1
#define LLVM_ENABLE_ZLIB 0
/* #undef LLVM_ETCDIR */
#define LLVM_HAS_ATOMICS 1
#if defined(_WIN32)
#define LLVM_HOST_TRIPLE "x86_64-pc-win32"
#elif defined(__APPLE__)
#define LLVM_HOST_TRIPLE "x86_64-unknown-linux-gnu"
#else
#define LLVM_HOST_TRIPLE "x86_64-unknown-linux-gnu"
#endif
/* #undef LLVM_INCLUDEDIR */
/* #undef LLVM_INFODIR */
/* #undef LLVM_MANDIR */
#define LLVM_NATIVE_ARCH X86
/* #undef LLVM_NATIVE_ASMPARSER */
/* #undef LLVM_NATIVE_ASMPRINTER */
/* #undef LLVM_NATIVE_DISASSEMBLER */
/* #undef LLVM_NATIVE_TARGET */
/* #undef LLVM_NATIVE_TARGETINFO */
/* #undef LLVM_NATIVE_TARGETMC */
#if defined(_WIN32)
/* #undef LLVM_ON_UNIX */
#elif defined(__APPLE__)
#define LLVM_ON_UNIX 1
#else
#define LLVM_ON_UNIX 1
#endif
#if defined(_WIN32)
#define LLVM_ON_WIN32 1
#elif defined(__APPLE__)
/* #undef LLVM_ON_WIN32 */
#else
/* #undef LLVM_ON_WIN32 */
#endif
#define LLVM_PREFIX "/usr/local"
/* #undef LLVM_USE_INTEL_JITEVENTS */
/* #undef LLVM_USE_OPROFILE */
#define LLVM_VERSION_MAJOR 3
#define LLVM_VERSION_MINOR 7
#define LLVM_VERSION_PATCH 0
#define LLVM_VERSION_STRING "3.7.0"
/* #undef LINK_POLLY_INTO_TOOLS */
/* #undef LTDL_DLOPEN_DEPLIBS */
#if defined(_WIN32)
#define LTDL_SHLIB_EXT ".dll"
#elif defined(__APPLE__)
#define LTDL_SHLIB_EXT ".dylib"
#else
#define LTDL_SHLIB_EXT ".so"
#endif
/* #undef LTDL_SYSSEARCHPATH */
#define PACKAGE_BUGREPORT "http://llvm.org/bugs/"
#define PACKAGE_NAME "LLVM"
#define PACKAGE_STRING "LLVM 3.7.0"
#define PACKAGE_VERSION "3.7.0"
#define RETSIGTYPE void
#if defined(_WIN32)
#define WIN32_ELMCB_PCSTR PCSTR
#elif defined(__APPLE__)
/* #undef WIN32_ELMCB_PCSTR */
#else
/* #undef WIN32_ELMCB_PCSTR */
#endif
/* #undef strtoll */
/* #undef strtoull */
#if defined(_WIN32)
#define stricmp _stricmp
#elif defined(__APPLE__)
/* #undef stricmp */
#else
/* #undef stricmp */
#endif
#if defined(_WIN32)
#define strdup _strdup
#elif defined(__APPLE__)
/* #undef strdup */
#else
/* #undef strdup */
#endif
#if defined(_WIN32)
#define HAVE__CHSIZE_S 1
#elif defined(__APPLE__)
/* #undef HAVE__CHSIZE_S */
#else
/* #undef HAVE__CHSIZE_S */
#endif

#endif
