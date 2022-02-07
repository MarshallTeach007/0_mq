#ifndef __SZMQ_PLATFORM_HPP_INCLUDED__
#define __SZMQ_PLATFORM_HPP_INCLUDED__

#define SZMQ_USE_CV_IMPL_STL11
/* #undef SZMQ_USE_CV_IMPL_WIN32API */
/* #undef SZMQ_USE_CV_IMPL_PTHREADS */
/* #undef SZMQ_USE_CV_IMPL_NONE */

/* #undef SZMQ_IOTHREAD_POLLER_USE_KQUEUE */
#define SZMQ_IOTHREAD_POLLER_USE_EPOLL
#define SZMQ_IOTHREAD_POLLER_USE_EPOLL_CLOEXEC
/* #undef SZMQ_IOTHREAD_POLLER_USE_DEVPOLL */
/* #undef SZMQ_IOTHREAD_POLLER_USE_POLL */
/* #undef SZMQ_IOTHREAD_POLLER_USE_SELECT */
#define SZMQ_HAVE_PPOLL

/* #undef SZMQ_POLL_BASED_ON_SELECT */
#define SZMQ_POLL_BASED_ON_POLL

#define HAVE_POSIX_MEMALIGN 1
#define SZMQ_CACHELINE_SIZE 64

/* #undef SZMQ_FORCE_MUTEXES */

#define HAVE_FORK
#define HAVE_CLOCK_GETTIME
/* #undef HAVE_GETHRTIME */
#define HAVE_MKDTEMP
#define SZMQ_HAVE_UIO

#define SZMQ_HAVE_NOEXCEPT

#define SZMQ_HAVE_EVENTFD
#define SZMQ_HAVE_EVENTFD_CLOEXEC
#define SZMQ_HAVE_IFADDRS
#define SZMQ_HAVE_SO_BINDTODEVICE

#define SZMQ_HAVE_SO_PEERCRED
/* #undef SZMQ_HAVE_LOCAL_PEERCRED */
#define SZMQ_HAVE_BUSY_POLL

/* #undef SZMQ_HAVE_O_CLOEXEC */

#define SZMQ_HAVE_SOCK_CLOEXEC
#define SZMQ_HAVE_SO_KEEPALIVE
#define SZMQ_HAVE_SO_PRIORITY
#define SZMQ_HAVE_TCP_KEEPCNT
#define SZMQ_HAVE_TCP_KEEPIDLE
#define SZMQ_HAVE_TCP_KEEPINTVL
/* #undef SZMQ_HAVE_TCP_KEEPALIVE */
/* #undef SZMQ_HAVE_PTHREAD_SETNAME_1 */
#define SZMQ_HAVE_PTHREAD_SETNAME_2
/* #undef SZMQ_HAVE_PTHREAD_SETNAME_3 */
/* #undef SZMQ_HAVE_PTHREAD_SET_NAME */
#define SZMQ_HAVE_PTHREAD_SET_AFFINITY
#define HAVE_ACCEPT4
#define HAVE_STRNLEN
/* #undef SZMQ_HAVE_STRLCPY */
/* #undef SZMQ_HAVE_LIBBSD */

#define SZMQ_HAVE_IPC
#define SZMQ_HAVE_STRUCT_SOCKADDR_UN

#define SZMQ_USE_BUILTIN_SHA1
/* #undef SZMQ_USE_NSS */
/* #undef SZMQ_HAVE_WS */
/* #undef SZMQ_HAVE_WSS */
#define SZMQ_HAVE_TIPC

/* #undef SZMQ_HAVE_OPENPGM */
/* #undef SZMQ_HAVE_NORM */
/* #undef SZMQ_MAKE_VALGRIND_HAPPY */

#define SZMQ_HAVE_CURVE
#define SZMQ_USE_TWEETNACL
/* #undef SZMQ_USE_LIBSODIUM */
/* #undef SODIUM_STATIC */
/* #undef SZMQ_USE_GNUTLS */
/* #undef SZMQ_USE_RADIX_TREE */
#define HAVE_IF_NAMETOINDEX

#ifdef _AIX
  #define SZMQ_HAVE_AIX
#endif

#if defined __ANDROID__
  #define SZMQ_HAVE_ANDROID
#endif

#if defined __CYGWIN__
  #define SZMQ_HAVE_CYGWIN
#endif

#if defined __MINGW32__
  #define SZMQ_HAVE_MINGW32
#endif

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
  #define SZMQ_HAVE_FREEBSD
#endif

#if defined(__DragonFly__)
  #define SZMQ_HAVE_FREEBSD
  #define SZMQ_HAVE_DRAGONFLY
#endif

#if defined __hpux
  #define SZMQ_HAVE_HPUX
#endif

#if defined __linux__
  #define SZMQ_HAVE_LINUX
#endif

#if defined __NetBSD__
  #define SZMQ_HAVE_NETBSD
#endif

#if defined __OpenBSD__
  #define SZMQ_HAVE_OPENBSD
#endif

// TODO better move OS-specific defines to the automake files, and check for availability of IPC with an explicit test there
#if defined __VMS
  #define SZMQ_HAVE_OPENVMS
  #undef SZMQ_HAVE_IPC
#endif

#if defined __APPLE__
  #define SZMQ_HAVE_OSX
#endif

#if defined __QNXNTO__
  #define SZMQ_HAVE_QNXNTO
#endif

#if defined(sun) || defined(__sun)
  #define SZMQ_HAVE_SOLARIS
#endif

/* #undef SZMQ_HAVE_WINDOWS */
/* #undef SZMQ_HAVE_WINDOWS_UWP */

#endif
