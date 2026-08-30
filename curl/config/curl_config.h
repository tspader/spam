#ifndef SPN_CURL_CONFIG_H
#define SPN_CURL_CONFIG_H

#ifdef _WIN32
  #ifndef SPN_CURL_THREADED_RESOLVER
    #define USE_SYNC_DNS 1
  #endif
  #include "config-win32.h"
  #undef USE_UNIX_SOCKETS
  #ifdef __MINGW32__
    #define HAVE_DIRENT_H 1
    #define HAVE_OPENDIR 1
  #endif
  #ifndef SPN_CURL_LDAP
    #undef USE_WIN32_LDAP
    #undef HAVE_LDAP_SSL
    #define CURL_DISABLE_LDAP 1
    #define CURL_DISABLE_LDAPS 1
  #endif
  #ifdef SPN_CURL_SCHANNEL
    #define USE_SCHANNEL 1
  #endif
  #if defined(SPN_CURL_SCHANNEL) || defined(SPN_CURL_SSPI)
    #define USE_WINDOWS_SSPI 1
  #endif
  #ifdef SPN_CURL_IDN
    #define USE_WIN32_IDN 1
  #endif
#else
  #if defined(__APPLE__)
    #define CURL_OS "Darwin"
    #define HAVE_ARC4RANDOM 1
    #define HAVE_BUILTIN_AVAILABLE 1
    #define HAVE_FSETXATTR_6 1
    #define HAVE_MACH_ABSOLUTE_TIME 1
    #define HAVE_POSIX_STRERROR_R 1
    #define HAVE_SYS_FILIO_H 1
    #define HAVE_SYS_SOCKIO_H 1
    #ifdef SPN_CURL_IDN
      #define USE_APPLE_IDN 1
    #endif
    #ifdef SPN_CURL_CA_BUNDLE
      #define CURL_CA_BUNDLE "/etc/ssl/cert.pem"
    #endif
  #elif defined(__linux__)
    #define _FILE_OFFSET_BITS 64
    #include <features.h>
    #define CURL_OS "Linux"
    #define HAVE_ACCEPT4 1
    #define HAVE_EVENTFD 1
    #define HAVE_FSETXATTR_5 1
    #define HAVE_GETHOSTBYNAME_R 1
    #define HAVE_GETHOSTBYNAME_R_6 1
    #define HAVE_MEMRCHR 1
    #define HAVE_PIPE2 1
    #define HAVE_SENDMMSG 1
    #define HAVE_SYS_EVENTFD_H 1
    #ifdef __GLIBC__
      #define HAVE_GLIBC_STRERROR_R 1
      #define HAVE_LINUX_TCP_H 1
      #if __GLIBC_PREREQ(2, 36)
        #define HAVE_ARC4RANDOM 1
      #endif
    #else
      #define HAVE_POSIX_STRERROR_R 1
    #endif
    #ifdef SPN_CURL_CA_BUNDLE
      #define CURL_CA_BUNDLE "/etc/ssl/certs/ca-certificates.crt"
    #endif
  #else
    #error "spn curl: unsupported platform"
  #endif

  #define CURL_DISABLE_LDAP 1
  #define CURL_DISABLE_LDAPS 1

  #define SIZEOF_INT __SIZEOF_INT__
  #define SIZEOF_LONG __SIZEOF_LONG__
  #define SIZEOF_SIZE_T __SIZEOF_SIZE_T__
  #define SIZEOF_TIME_T __SIZEOF_POINTER__
  #define SIZEOF_OFF_T 8
  #define SIZEOF_CURL_OFF_T 8
  #define SIZEOF_CURL_SOCKET_T 4

  #define HAVE_ALARM 1
  #define HAVE_ARPA_INET_H 1
  #define HAVE_ATOMIC 1
  #define HAVE_BASENAME 1
  #define HAVE_BOOL_T 1
  #define HAVE_CLOCK_GETTIME_MONOTONIC 1
  #define HAVE_CLOCK_GETTIME_MONOTONIC_RAW 1
  #define HAVE_DECL_FSEEKO 1
  #define HAVE_DIRENT_H 1
  #define HAVE_FCNTL 1
  #define HAVE_FCNTL_H 1
  #define HAVE_FCNTL_O_NONBLOCK 1
  #define HAVE_FNMATCH 1
  #define HAVE_FREEADDRINFO 1
  #define HAVE_FSEEKO 1
  #define HAVE_FSETXATTR 1
  #define HAVE_FTRUNCATE 1
  #define HAVE_GETADDRINFO 1
  #define HAVE_GETADDRINFO_THREADSAFE 1
  #define HAVE_GETEUID 1
  #define HAVE_GETHOSTNAME 1
  #define HAVE_GETIFADDRS 1
  #define HAVE_GETPEERNAME 1
  #define HAVE_GETPPID 1
  #define HAVE_GETPWUID 1
  #define HAVE_GETPWUID_R 1
  #define HAVE_GETRLIMIT 1
  #define HAVE_GETSOCKNAME 1
  #define HAVE_GETTIMEOFDAY 1
  #define HAVE_GMTIME_R 1
  #define HAVE_IFADDRS_H 1
  #define HAVE_IF_NAMETOINDEX 1
  #define HAVE_INET_NTOP 1
  #define HAVE_INET_PTON 1
  #define HAVE_IOCTL_FIONBIO 1
  #define HAVE_IOCTL_SIOCGIFADDR 1
  #define HAVE_LIBGEN_H 1
  #define HAVE_LOCALE_H 1
  #define HAVE_LOCALTIME_R 1
  #define HAVE_NETDB_H 1
  #define HAVE_NETINET_IN_H 1
  #define HAVE_NETINET_TCP_H 1
  #define HAVE_NETINET_UDP_H 1
  #define HAVE_NET_IF_H 1
  #define HAVE_OPENDIR 1
  #define HAVE_PIPE 1
  #define HAVE_POLL 1
  #define HAVE_POLL_H 1
  #define HAVE_PWD_H 1
  #define HAVE_REALPATH 1
  #define HAVE_RECV 1
  #define HAVE_SA_FAMILY_T 1
  #define HAVE_SCHED_YIELD 1
  #define HAVE_SELECT 1
  #define HAVE_SEND 1
  #define HAVE_SENDMSG 1
  #define HAVE_SETLOCALE 1
  #define HAVE_SETRLIMIT 1
  #define HAVE_SIGACTION 1
  #define HAVE_SIGINTERRUPT 1
  #define HAVE_SIGNAL 1
  #define HAVE_SIGSETJMP 1
  #define HAVE_SNPRINTF 1
  #define HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1
  #define HAVE_SOCKET 1
  #define HAVE_SOCKETPAIR 1
  #define HAVE_STDATOMIC_H 1
  #define HAVE_STDBOOL_H 1
  #define HAVE_STRCASECMP 1
  #define HAVE_STRERROR_R 1
  #define HAVE_STRINGS_H 1
  #define HAVE_STRUCT_SOCKADDR_STORAGE 1
  #define HAVE_STRUCT_TIMEVAL 1
  #define HAVE_SUSECONDS_T 1
  #define HAVE_SYS_IOCTL_H 1
  #define HAVE_SYS_PARAM_H 1
  #define HAVE_SYS_POLL_H 1
  #define HAVE_SYS_RESOURCE_H 1
  #define HAVE_SYS_SELECT_H 1
  #define HAVE_SYS_TYPES_H 1
  #define HAVE_SYS_UN_H 1
  #define HAVE_TERMIOS_H 1
  #define HAVE_UNISTD_H 1
  #define HAVE_UTIME 1
  #define HAVE_UTIMES 1
  #define HAVE_UTIME_H 1
  #define HAVE_WRITABLE_ARGV 1
  #define STDC_HEADERS 1

  #ifdef SPN_CURL_THREADED_RESOLVER
    #define USE_THREADS_POSIX 1
    #define HAVE_PTHREAD_H 1
  #endif
#endif

#ifdef SPN_CURL_UNIX_SOCKETS
  #define USE_UNIX_SOCKETS 1
#endif
#ifdef SPN_CURL_IPV6
  #define USE_IPV6 1
#endif
#ifdef SPN_CURL_MBEDTLS
  #define USE_MBEDTLS 1
  #define HAVE_MBEDTLS_DES_CRYPT_ECB 1
#endif

#ifndef SPN_CURL_HTTP
  #define CURL_DISABLE_HTTP 1
  #define CURL_DISABLE_IPFS 1
#endif
#ifndef SPN_CURL_FTP
  #define CURL_DISABLE_FTP 1
#endif
#ifndef SPN_CURL_FILE
  #define CURL_DISABLE_FILE 1
#endif
#ifndef SPN_CURL_DICT
  #define CURL_DISABLE_DICT 1
#endif
#ifndef SPN_CURL_GOPHER
  #define CURL_DISABLE_GOPHER 1
#endif
#ifndef SPN_CURL_IMAP
  #define CURL_DISABLE_IMAP 1
#endif
#ifndef SPN_CURL_POP3
  #define CURL_DISABLE_POP3 1
#endif
#ifndef SPN_CURL_SMTP
  #define CURL_DISABLE_SMTP 1
#endif
#ifndef SPN_CURL_TELNET
  #define CURL_DISABLE_TELNET 1
#endif
#ifndef SPN_CURL_TFTP
  #define CURL_DISABLE_TFTP 1
#endif
#ifndef SPN_CURL_MQTT
  #define CURL_DISABLE_MQTT 1
#endif
#ifndef SPN_CURL_RTSP
  #define CURL_DISABLE_RTSP 1
#endif
#ifndef SPN_CURL_SMB
  #define CURL_DISABLE_SMB 1
#endif
#ifndef SPN_CURL_IPFS
  #define CURL_DISABLE_IPFS 1
#endif
#ifndef SPN_CURL_WEBSOCKETS
  #define CURL_DISABLE_WEBSOCKETS 1
#endif
#ifndef SPN_CURL_PROXY
  #define CURL_DISABLE_PROXY 1
#endif
#ifndef SPN_CURL_COOKIES
  #define CURL_DISABLE_COOKIES 1
#endif
#ifndef SPN_CURL_DOH
  #define CURL_DISABLE_DOH 1
#endif
#ifndef SPN_CURL_HSTS
  #define CURL_DISABLE_HSTS 1
#endif
#ifndef SPN_CURL_ALTSVC
  #define CURL_DISABLE_ALTSVC 1
#endif
#ifndef SPN_CURL_NETRC
  #define CURL_DISABLE_NETRC 1
#endif
#ifndef SPN_CURL_MIME
  #define CURL_DISABLE_MIME 1
#endif
#ifndef SPN_CURL_FORM_API
  #define CURL_DISABLE_FORM_API 1
#endif
#ifndef SPN_CURL_NTLM
  #define CURL_DISABLE_NTLM 1
#endif
#ifndef SPN_CURL_AWS
  #define CURL_DISABLE_AWS 1
#endif
#ifndef SPN_CURL_HTTP_AUTH
  #define CURL_DISABLE_HTTP_AUTH 1
#endif
#ifndef SPN_CURL_BASIC_AUTH
  #define CURL_DISABLE_BASIC_AUTH 1
#endif
#ifndef SPN_CURL_BEARER_AUTH
  #define CURL_DISABLE_BEARER_AUTH 1
#endif
#ifndef SPN_CURL_DIGEST_AUTH
  #define CURL_DISABLE_DIGEST_AUTH 1
#endif
#ifndef SPN_CURL_KERBEROS_AUTH
  #define CURL_DISABLE_KERBEROS_AUTH 1
#endif
#ifndef SPN_CURL_NEGOTIATE_AUTH
  #define CURL_DISABLE_NEGOTIATE_AUTH 1
#endif
#ifndef SPN_CURL_VERBOSE_STRINGS
  #define CURL_DISABLE_VERBOSE_STRINGS 1
#endif
#if !defined(USE_MBEDTLS) && !defined(USE_SCHANNEL)
  #define CURL_DISABLE_HSTS 1
#endif

#endif
