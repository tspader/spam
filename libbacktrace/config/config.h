#ifndef SPN_LIBBACKTRACE_CONFIG_H
#define SPN_LIBBACKTRACE_CONFIG_H

#ifndef _GNU_SOURCE
  #define _GNU_SOURCE 1
#endif

#define HAVE_ATOMIC_FUNCTIONS 1
#define HAVE_SYNC_FUNCTIONS 1
#define HAVE_GETIPINFO 1
#define HAVE_DECL_STRNLEN 1

#if defined(_WIN32)
  #define HAVE_WINDOWS_H 1
  #define HAVE_TLHELP32_H 1
  #define HAVE_DECL__PGMPTR 1
  #define HAVE_DECL_GETPAGESIZE 0
#else
  #define HAVE_DECL__PGMPTR 0
  #define HAVE_DECL_GETPAGESIZE 1
  #define HAVE_FCNTL 1
  #define HAVE_LSTAT 1
  #define HAVE_READLINK 1
  #if defined(__APPLE__)
    #define HAVE_MACH_O_DYLD_H 1
  #else
    #define HAVE_DL_ITERATE_PHDR 1
    #define HAVE_LINK_H 1
    #if __SIZEOF_POINTER__ == 8
      #define BACKTRACE_ELF_SIZE 64
    #else
      #define BACKTRACE_ELF_SIZE 32
    #endif
  #endif
#endif

#endif
