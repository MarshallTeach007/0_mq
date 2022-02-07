

#ifndef __SZMQ_H_INCLUDED__
#define __SZMQ_H_INCLUDED__

/*  Version macros for compile-time API version detection                     */
#define SZMQ_VERSION_MAJOR 4
#define SZMQ_VERSION_MINOR 3
#define SZMQ_VERSION_PATCH 5

#define SZMQ_MAKE_VERSION(major, minor, patch)                                  \
    ((major) *10000 + (minor) *100 + (patch))
#define SZMQ_VERSION                                                            \
    SZMQ_MAKE_VERSION (SZMQ_VERSION_MAJOR, SZMQ_VERSION_MINOR, SZMQ_VERSION_PATCH)

#ifdef __cplusplus
extern "C" {
#endif

#if !defined _WIN32_WCE
#include <errno.h>
#endif
#include <stddef.h>
#include <stdio.h>

/*  Handle DSO symbol visibility                                             */
#if defined SZMQ_NO_EXPORT
#define SZMQ_EXPORT
#else
#if defined _WIN32
#if defined SZMQ_STATIC
#define SZMQ_EXPORT
#elif defined DLL_EXPORT
#define SZMQ_EXPORT __declspec(dllexport)
#else
#define SZMQ_EXPORT __declspec(dllimport)
#endif
#else
#if defined __SUNPRO_C || defined __SUNPRO_CC
#define SZMQ_EXPORT __global
#elif (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#define SZMQ_EXPORT __attribute__ ((visibility ("default")))
#else
#define SZMQ_EXPORT
#endif
#endif
#endif

/*  Define integer types needed for event interface                          */
#define SZMQ_DEFINED_STDINT 1
#if defined SZMQ_HAVE_SOLARIS || defined SZMQ_HAVE_OPENVMS
#include <inttypes.h>
#elif defined _MSC_VER && _MSC_VER < 1600
#ifndef uint64_t
typedef unsigned __int64 uint64_t;
#endif
#ifndef int32_t
typedef __int32 int32_t;
#endif
#ifndef uint32_t
typedef unsigned __int32 uint32_t;
#endif
#ifndef uint16_t
typedef unsigned __int16 uint16_t;
#endif
#ifndef uint8_t
typedef unsigned __int8 uint8_t;
#endif
#else
#include <stdint.h>
#endif

#if !defined _WIN32
// needed for sigset_t definition in zmq_ppoll
#include <signal.h>
#endif

//  32-bit AIX's pollfd struct members are called reqevents and rtnevents so it
//  defines compatibility macros for them. Need to include that header first to
//  stop build failures since zmq_pollset_t defines them as events and revents.
#ifdef SZMQ_HAVE_AIX
#include <poll.h>
#endif


/******************************************************************************/
/*  0MQ errors.                                                               */
/******************************************************************************/

/*  A number random enough not to collide with different errno ranges on      */
/*  different OSes. The assumption is that error_t is at least 32-bit type.   */
#define SZMQ_HAUSNUMERO 156384712

/*  On Windows platform some of the standard POSIX errnos are not defined.    */
#ifndef ENOTSUP
#define ENOTSUP (SZMQ_HAUSNUMERO + 1)
#endif
#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT (SZMQ_HAUSNUMERO + 2)
#endif
#ifndef ENOBUFS
#define ENOBUFS (SZMQ_HAUSNUMERO + 3)
#endif
#ifndef ENETDOWN
#define ENETDOWN (SZMQ_HAUSNUMERO + 4)
#endif
#ifndef EADDRINUSE
#define EADDRINUSE (SZMQ_HAUSNUMERO + 5)
#endif
#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL (SZMQ_HAUSNUMERO + 6)
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED (SZMQ_HAUSNUMERO + 7)
#endif
#ifndef EINPROGRESS
#define EINPROGRESS (SZMQ_HAUSNUMERO + 8)
#endif
#ifndef ENOTSOCK
#define ENOTSOCK (SZMQ_HAUSNUMERO + 9)
#endif
#ifndef EMSGSIZE
#define EMSGSIZE (SZMQ_HAUSNUMERO + 10)
#endif
#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT (SZMQ_HAUSNUMERO + 11)
#endif
#ifndef ENETUNREACH
#define ENETUNREACH (SZMQ_HAUSNUMERO + 12)
#endif
#ifndef ECONNABORTED
#define ECONNABORTED (SZMQ_HAUSNUMERO + 13)
#endif
#ifndef ECONNRESET
#define ECONNRESET (SZMQ_HAUSNUMERO + 14)
#endif
#ifndef ENOTCONN
#define ENOTCONN (SZMQ_HAUSNUMERO + 15)
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT (SZMQ_HAUSNUMERO + 16)
#endif
#ifndef EHOSTUNREACH
#define EHOSTUNREACH (SZMQ_HAUSNUMERO + 17)
#endif
#ifndef ENETRESET
#define ENETRESET (SZMQ_HAUSNUMERO + 18)
#endif

/*  Native 0MQ error codes.                                                   */
#define EFSM (SZMQ_HAUSNUMERO + 51)
#define ENOCOMPATPROTO (SZMQ_HAUSNUMERO + 52)
#define ETERM (SZMQ_HAUSNUMERO + 53)
#define EMTHREAD (SZMQ_HAUSNUMERO + 54)

/*  This function retrieves the errno as it is known to 0MQ library. The goal */
/*  of this function is to make the code 100% portable, including where 0MQ   */
/*  compiled with certain CRT library (on Windows) is linked to an            */
/*  application that uses different CRT library.                              */
SZMQ_EXPORT int zmq_errno (void);

/*  Resolves system errors and 0MQ errors to human-readable string.           */
SZMQ_EXPORT const char *zmq_strerror (int errnum_);

/*  Run-time API version detection                                            */
SZMQ_EXPORT void zmq_version (int *major_, int *minor_, int *patch_);

/******************************************************************************/
/*  0MQ infrastructure (a.k.a. context) initialisation & termination.         */
/******************************************************************************/

/*  Context options                                                           */
#define SZMQ_IO_THREADS 1
#define SZMQ_MAX_SOCKETS 2
#define SZMQ_SOCKET_LIMIT 3
#define SZMQ_THREAD_PRIORITY 3
#define SZMQ_THREAD_SCHED_POLICY 4
#define SZMQ_MAX_MSGSZ 5
#define SZMQ_MSG_T_SIZE 6
#define SZMQ_THREAD_AFFINITY_CPU_ADD 7
#define SZMQ_THREAD_AFFINITY_CPU_REMOVE 8
#define SZMQ_THREAD_NAME_PREFIX 9

/*  Default for new contexts                                                  */
#define SZMQ_IO_THREADS_DFLT 1
#define SZMQ_MAX_SOCKETS_DFLT 1023
#define SZMQ_THREAD_PRIORITY_DFLT -1
#define SZMQ_THREAD_SCHED_POLICY_DFLT -1

SZMQ_EXPORT void *zmq_ctx_new (void);
SZMQ_EXPORT int zmq_ctx_term (void *context_);
SZMQ_EXPORT int zmq_ctx_shutdown (void *context_);
SZMQ_EXPORT int zmq_ctx_set (void *context_, int option_, int optval_);
SZMQ_EXPORT int zmq_ctx_get (void *context_, int option_);

/*  Old (legacy) API                                                          */
SZMQ_EXPORT void *zmq_init (int io_threads_);
SZMQ_EXPORT int zmq_term (void *context_);
SZMQ_EXPORT int zmq_ctx_destroy (void *context_);


/******************************************************************************/
/*  0MQ message definition.                                                   */
/******************************************************************************/

/* Some architectures, like sparc64 and some variants of aarch64, enforce pointer
 * alignment and raise sigbus on violations. Make sure applications allocate
 * zmq_msg_t on addresses aligned on a pointer-size boundary to avoid this issue.
 */
typedef struct zmq_msg_t
{
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    __declspec(align (8)) unsigned char _[64];
#elif defined(_MSC_VER)                                                        \
  && (defined(_M_IX86) || defined(_M_ARM_ARMV7VE) || defined(_M_ARM))
    __declspec(align (4)) unsigned char _[64];
#elif defined(__GNUC__) || defined(__INTEL_COMPILER)                           \
  || (defined(__SUNPRO_C) && __SUNPRO_C >= 0x590)                              \
  || (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x590)
    unsigned char _[64] __attribute__ ((aligned (sizeof (void *))));
#else
    unsigned char _[64];
#endif
} zmq_msg_t;

typedef void (zmq_free_fn) (void *data_, void *hint_);

SZMQ_EXPORT int zmq_msg_init (zmq_msg_t *msg_);
SZMQ_EXPORT int zmq_msg_init_size (zmq_msg_t *msg_, size_t size_);
SZMQ_EXPORT int zmq_msg_init_data (
  zmq_msg_t *msg_, void *data_, size_t size_, zmq_free_fn *ffn_, void *hint_);
SZMQ_EXPORT int zmq_msg_send (zmq_msg_t *msg_, void *s_, int flags_);
SZMQ_EXPORT int zmq_msg_recv (zmq_msg_t *msg_, void *s_, int flags_);
SZMQ_EXPORT int zmq_msg_close (zmq_msg_t *msg_);
SZMQ_EXPORT int zmq_msg_move (zmq_msg_t *dest_, zmq_msg_t *src_);
SZMQ_EXPORT int zmq_msg_copy (zmq_msg_t *dest_, zmq_msg_t *src_);
SZMQ_EXPORT void *zmq_msg_data (zmq_msg_t *msg_);
SZMQ_EXPORT size_t zmq_msg_size (const zmq_msg_t *msg_);
SZMQ_EXPORT int zmq_msg_more (const zmq_msg_t *msg_);
SZMQ_EXPORT int zmq_msg_get (const zmq_msg_t *msg_, int property_);
SZMQ_EXPORT int zmq_msg_set (zmq_msg_t *msg_, int property_, int optval_);
SZMQ_EXPORT const char *zmq_msg_gets (const zmq_msg_t *msg_,
                                     const char *property_);

/******************************************************************************/
/*  0MQ socket definition.                                                    */
/******************************************************************************/

/*  Socket types.                                                             */
#define SZMQ_PAIR 0
#define SZMQ_PUB 1
#define SZMQ_SUB 2
#define SZMQ_REQ 3
#define SZMQ_REP 4
#define SZMQ_DEALER 5
#define SZMQ_ROUTER 6
#define SZMQ_PULL 7
#define SZMQ_PUSH 8
#define SZMQ_XPUB 9
#define SZMQ_XSUB 10
#define SZMQ_STREAM 11

/*  Deprecated aliases                                                        */
#define SZMQ_XREQ SZMQ_DEALER
#define SZMQ_XREP SZMQ_ROUTER

/*  Socket options.                                                           */
#define SZMQ_AFFINITY 4
#define SZMQ_ROUTING_ID 5
#define SZMQ_SUBSCRIBE 6
#define SZMQ_UNSUBSCRIBE 7
#define SZMQ_RATE 8
#define SZMQ_RECOVERY_IVL 9
#define SZMQ_SNDBUF 11
#define SZMQ_RCVBUF 12
#define SZMQ_RCVMORE 13
#define SZMQ_FD 14
#define SZMQ_EVENTS 15
#define SZMQ_TYPE 16
#define SZMQ_LINGER 17
#define SZMQ_RECONNECT_IVL 18
#define SZMQ_BACKLOG 19
#define SZMQ_RECONNECT_IVL_MAX 21
#define SZMQ_MAXMSGSIZE 22
#define SZMQ_SNDHWM 23
#define SZMQ_RCVHWM 24
#define SZMQ_MULTICAST_HOPS 25
#define SZMQ_RCVTIMEO 27
#define SZMQ_SNDTIMEO 28
#define SZMQ_LAST_ENDPOINT 32
#define SZMQ_ROUTER_MANDATORY 33
#define SZMQ_TCP_KEEPALIVE 34
#define SZMQ_TCP_KEEPALIVE_CNT 35
#define SZMQ_TCP_KEEPALIVE_IDLE 36
#define SZMQ_TCP_KEEPALIVE_INTVL 37
#define SZMQ_IMMEDIATE 39
#define SZMQ_XPUB_VERBOSE 40
#define SZMQ_ROUTER_RAW 41
#define SZMQ_IPV6 42
#define SZMQ_MECHANISM 43
#define SZMQ_PLAIN_SERVER 44
#define SZMQ_PLAIN_USERNAME 45
#define SZMQ_PLAIN_PASSWORD 46
#define SZMQ_CURVE_SERVER 47
#define SZMQ_CURVE_PUBLICKEY 48
#define SZMQ_CURVE_SECRETKEY 49
#define SZMQ_CURVE_SERVERKEY 50
#define SZMQ_PROBE_ROUTER 51
#define SZMQ_REQ_CORRELATE 52
#define SZMQ_REQ_RELAXED 53
#define SZMQ_CONFLATE 54
#define SZMQ_ZAP_DOMAIN 55
#define SZMQ_ROUTER_HANDOVER 56
#define SZMQ_TOS 57
#define SZMQ_CONNECT_ROUTING_ID 61
#define SZMQ_GSSAPI_SERVER 62
#define SZMQ_GSSAPI_PRINCIPAL 63
#define SZMQ_GSSAPI_SERVICE_PRINCIPAL 64
#define SZMQ_GSSAPI_PLAINTEXT 65
#define SZMQ_HANDSHAKE_IVL 66
#define SZMQ_SOCKS_PROXY 68
#define SZMQ_XPUB_NODROP 69
#define SZMQ_BLOCKY 70
#define SZMQ_XPUB_MANUAL 71
#define SZMQ_XPUB_WELCOME_MSG 72
#define SZMQ_STREAM_NOTIFY 73
#define SZMQ_INVERT_MATCHING 74
#define SZMQ_HEARTBEAT_IVL 75
#define SZMQ_HEARTBEAT_TTL 76
#define SZMQ_HEARTBEAT_TIMEOUT 77
#define SZMQ_XPUB_VERBOSER 78
#define SZMQ_CONNECT_TIMEOUT 79
#define SZMQ_TCP_MAXRT 80
#define SZMQ_THREAD_SAFE 81
#define SZMQ_MULTICAST_MAXTPDU 84
#define SZMQ_VMCI_BUFFER_SIZE 85
#define SZMQ_VMCI_BUFFER_MIN_SIZE 86
#define SZMQ_VMCI_BUFFER_MAX_SIZE 87
#define SZMQ_VMCI_CONNECT_TIMEOUT 88
#define SZMQ_USE_FD 89
#define SZMQ_GSSAPI_PRINCIPAL_NAMETYPE 90
#define SZMQ_GSSAPI_SERVICE_PRINCIPAL_NAMETYPE 91
#define SZMQ_BINDTODEVICE 92

/*  Message options                                                           */
#define SZMQ_MORE 1
#define SZMQ_SHARED 3

/*  Send/recv options.                                                        */
#define SZMQ_DONTWAIT 1
#define SZMQ_SNDMORE 2

/*  Security mechanisms                                                       */
#define SZMQ_NULL 0
#define SZMQ_PLAIN 1
#define SZMQ_CURVE 2
#define SZMQ_GSSAPI 3

/*  RADIO-DISH protocol                                                       */
#define SZMQ_GROUP_MAX_LENGTH 255

/*  Deprecated options and aliases                                            */
#define SZMQ_IDENTITY SZMQ_ROUTING_ID
#define SZMQ_CONNECT_RID SZMQ_CONNECT_ROUTING_ID
#define SZMQ_TCP_ACCEPT_FILTER 38
#define SZMQ_IPC_FILTER_PID 58
#define SZMQ_IPC_FILTER_UID 59
#define SZMQ_IPC_FILTER_GID 60
#define SZMQ_IPV4ONLY 31
#define SZMQ_DELAY_ATTACH_ON_CONNECT SZMQ_IMMEDIATE
#define SZMQ_NOBLOCK SZMQ_DONTWAIT
#define SZMQ_FAIL_UNROUTABLE SZMQ_ROUTER_MANDATORY
#define SZMQ_ROUTER_BEHAVIOR SZMQ_ROUTER_MANDATORY

/*  Deprecated Message options                                                */
#define SZMQ_SRCFD 2

/******************************************************************************/
/*  GSSAPI definitions                                                        */
/******************************************************************************/

/*  GSSAPI principal name types                                               */
#define SZMQ_GSSAPI_NT_HOSTBASED 0
#define SZMQ_GSSAPI_NT_USER_NAME 1
#define SZMQ_GSSAPI_NT_KRB5_PRINCIPAL 2

/******************************************************************************/
/*  0MQ socket events and monitoring                                          */
/******************************************************************************/

/*  Socket transport events (TCP, IPC and TIPC only)                          */

#define SZMQ_EVENT_CONNECTED 0x0001
#define SZMQ_EVENT_CONNECT_DELAYED 0x0002
#define SZMQ_EVENT_CONNECT_RETRIED 0x0004
#define SZMQ_EVENT_LISTENING 0x0008
#define SZMQ_EVENT_BIND_FAILED 0x0010
#define SZMQ_EVENT_ACCEPTED 0x0020
#define SZMQ_EVENT_ACCEPT_FAILED 0x0040
#define SZMQ_EVENT_CLOSED 0x0080
#define SZMQ_EVENT_CLOSE_FAILED 0x0100
#define SZMQ_EVENT_DISCONNECTED 0x0200
#define SZMQ_EVENT_MONITOR_STOPPED 0x0400
#define SZMQ_EVENT_ALL 0xFFFF
/*  Unspecified system errors during handshake. Event value is an errno.      */
#define SZMQ_EVENT_HANDSHAKE_FAILED_NO_DETAIL 0x0800
/*  Handshake complete successfully with successful authentication (if        *
 *  enabled). Event value is unused.                                          */
#define SZMQ_EVENT_HANDSHAKE_SUCCEEDED 0x1000
/*  Protocol errors between ZMTP peers or between server and ZAP handler.     *
 *  Event value is one of SZMQ_PROTOCOL_ERROR_*                                */
#define SZMQ_EVENT_HANDSHAKE_FAILED_PROTOCOL 0x2000
/*  Failed authentication requests. Event value is the numeric ZAP status     *
 *  code, i.e. 300, 400 or 500.                                               */
#define SZMQ_EVENT_HANDSHAKE_FAILED_AUTH 0x4000
#define SZMQ_PROTOCOL_ERROR_ZMTP_UNSPECIFIED 0x10000000
#define SZMQ_PROTOCOL_ERROR_ZMTP_UNEXPECTED_COMMAND 0x10000001
#define SZMQ_PROTOCOL_ERROR_ZMTP_INVALID_SEQUENCE 0x10000002
#define SZMQ_PROTOCOL_ERROR_ZMTP_KEY_EXCHANGE 0x10000003
#define SZMQ_PROTOCOL_ERROR_ZMTP_MALFORMED_COMMAND_UNSPECIFIED 0x10000011
#define SZMQ_PROTOCOL_ERROR_ZMTP_MALFORMED_COMMAND_MESSAGE 0x10000012
#define SZMQ_PROTOCOL_ERROR_ZMTP_MALFORMED_COMMAND_HELLO 0x10000013
#define SZMQ_PROTOCOL_ERROR_ZMTP_MALFORMED_COMMAND_INITIATE 0x10000014
#define SZMQ_PROTOCOL_ERROR_ZMTP_MALFORMED_COMMAND_ERROR 0x10000015
#define SZMQ_PROTOCOL_ERROR_ZMTP_MALFORMED_COMMAND_READY 0x10000016
#define SZMQ_PROTOCOL_ERROR_ZMTP_MALFORMED_COMMAND_WELCOME 0x10000017
#define SZMQ_PROTOCOL_ERROR_ZMTP_INVALID_METADATA 0x10000018
// the following two may be due to erroneous configuration of a peer
#define SZMQ_PROTOCOL_ERROR_ZMTP_CRYPTOGRAPHIC 0x11000001
#define SZMQ_PROTOCOL_ERROR_ZMTP_MECHANISM_MISMATCH 0x11000002
#define SZMQ_PROTOCOL_ERROR_ZAP_UNSPECIFIED 0x20000000
#define SZMQ_PROTOCOL_ERROR_ZAP_MALFORMED_REPLY 0x20000001
#define SZMQ_PROTOCOL_ERROR_ZAP_BAD_REQUEST_ID 0x20000002
#define SZMQ_PROTOCOL_ERROR_ZAP_BAD_VERSION 0x20000003
#define SZMQ_PROTOCOL_ERROR_ZAP_INVALID_STATUS_CODE 0x20000004
#define SZMQ_PROTOCOL_ERROR_ZAP_INVALID_METADATA 0x20000005
#define SZMQ_PROTOCOL_ERROR_WS_UNSPECIFIED 0x30000000

SZMQ_EXPORT void *zmq_socket (void *, int type_);
SZMQ_EXPORT int zmq_close (void *s_);
SZMQ_EXPORT int
zmq_setsockopt (void *s_, int option_, const void *optval_, size_t optvallen_);
SZMQ_EXPORT int
zmq_getsockopt (void *s_, int option_, void *optval_, size_t *optvallen_);
SZMQ_EXPORT int zmq_bind (void *s_, const char *addr_);
SZMQ_EXPORT int zmq_connect (void *s_, const char *addr_);
SZMQ_EXPORT int zmq_unbind (void *s_, const char *addr_);
SZMQ_EXPORT int zmq_disconnect (void *s_, const char *addr_);
SZMQ_EXPORT int zmq_send (void *s_, const void *buf_, size_t len_, int flags_);
SZMQ_EXPORT int
zmq_send_const (void *s_, const void *buf_, size_t len_, int flags_);
SZMQ_EXPORT int zmq_recv (void *s_, void *buf_, size_t len_, int flags_);
SZMQ_EXPORT int zmq_socket_monitor (void *s_, const char *addr_, int events_);

/******************************************************************************/
/*  Hide socket fd type; this was before zmq_poller_event_t typedef below     */
/******************************************************************************/

#if defined _WIN32
// Windows uses a pointer-sized unsigned integer to store the socket fd.
#if defined _WIN64
typedef unsigned __int64 zmq_fd_t;
#else
typedef unsigned int zmq_fd_t;
#endif
#else
typedef int zmq_fd_t;
#endif

/******************************************************************************/
/*  Deprecated I/O multiplexing. Prefer using zmq_poller API                  */
/******************************************************************************/

#define SZMQ_POLLIN 1
#define SZMQ_POLLOUT 2
#define SZMQ_POLLERR 4
#define SZMQ_POLLPRI 8

typedef struct zmq_pollitem_t
{
    void *socket;
    zmq_fd_t fd;
    short events;
    short revents;
} zmq_pollitem_t;

#define SZMQ_POLLITEMS_DFLT 16

SZMQ_EXPORT int zmq_poll (zmq_pollitem_t *items_, int nitems_, long timeout_);

/******************************************************************************/
/*  Message proxying                                                          */
/******************************************************************************/

SZMQ_EXPORT int zmq_proxy (void *frontend_, void *backend_, void *capture_);
SZMQ_EXPORT int zmq_proxy_steerable (void *frontend_,
                                    void *backend_,
                                    void *capture_,
                                    void *control_);

/******************************************************************************/
/*  Probe library capabilities                                                */
/******************************************************************************/

#define SZMQ_HAS_CAPABILITIES 1
SZMQ_EXPORT int zmq_has (const char *capability_);

/*  Deprecated aliases */
#define SZMQ_STREAMER 1
#define SZMQ_FORWARDER 2
#define SZMQ_QUEUE 3

/*  Deprecated methods */
SZMQ_EXPORT int zmq_device (int type_, void *frontend_, void *backend_);
SZMQ_EXPORT int zmq_sendmsg (void *s_, zmq_msg_t *msg_, int flags_);
SZMQ_EXPORT int zmq_recvmsg (void *s_, zmq_msg_t *msg_, int flags_);
struct iovec;
SZMQ_EXPORT int
zmq_sendiov (void *s_, struct iovec *iov_, size_t count_, int flags_);
SZMQ_EXPORT int
zmq_recviov (void *s_, struct iovec *iov_, size_t *count_, int flags_);

/******************************************************************************/
/*  Encryption functions                                                      */
/******************************************************************************/

/*  Encode data with Z85 encoding. Returns encoded data                       */
SZMQ_EXPORT char *
zmq_z85_encode (char *dest_, const uint8_t *data_, size_t size_);

/*  Decode data with Z85 encoding. Returns decoded data                       */
SZMQ_EXPORT uint8_t *zmq_z85_decode (uint8_t *dest_, const char *string_);

/*  Generate z85-encoded public and private keypair with tweetnacl/libsodium. */
/*  Returns 0 on success.                                                     */
SZMQ_EXPORT int zmq_curve_keypair (char *z85_public_key_, char *z85_secret_key_);

/*  Derive the z85-encoded public key from the z85-encoded secret key.        */
/*  Returns 0 on success.                                                     */
SZMQ_EXPORT int zmq_curve_public (char *z85_public_key_,
                                 const char *z85_secret_key_);

/******************************************************************************/
/*  Atomic utility methods                                                    */
/******************************************************************************/

SZMQ_EXPORT void *zmq_atomic_counter_new (void);
SZMQ_EXPORT void zmq_atomic_counter_set (void *counter_, int value_);
SZMQ_EXPORT int zmq_atomic_counter_inc (void *counter_);
SZMQ_EXPORT int zmq_atomic_counter_dec (void *counter_);
SZMQ_EXPORT int zmq_atomic_counter_value (void *counter_);
SZMQ_EXPORT void zmq_atomic_counter_destroy (void **counter_p_);

/******************************************************************************/
/*  Scheduling timers                                                         */
/******************************************************************************/

#define SZMQ_HAVE_TIMERS

typedef void (zmq_timer_fn) (int timer_id, void *arg);

SZMQ_EXPORT void *zmq_timers_new (void);
SZMQ_EXPORT int zmq_timers_destroy (void **timers_p);
SZMQ_EXPORT int
zmq_timers_add (void *timers, size_t interval, zmq_timer_fn handler, void *arg);
SZMQ_EXPORT int zmq_timers_cancel (void *timers, int timer_id);
SZMQ_EXPORT int
zmq_timers_set_interval (void *timers, int timer_id, size_t interval);
SZMQ_EXPORT int zmq_timers_reset (void *timers, int timer_id);
SZMQ_EXPORT long zmq_timers_timeout (void *timers);
SZMQ_EXPORT int zmq_timers_execute (void *timers);


/******************************************************************************/
/*  These functions are not documented by man pages -- use at your own risk.  */
/*  If you need these to be part of the formal ZMQ API, then (a) write a man  */
/*  page, and (b) write a test case in tests.                                 */
/******************************************************************************/

/*  Helper functions are used by perf tests so that they don't have to care   */
/*  about minutiae of time-related functions on different OS platforms.       */

/*  Starts the stopwatch. Returns the handle to the watch.                    */
SZMQ_EXPORT void *zmq_stopwatch_start (void);

/*  Returns the number of microseconds elapsed since the stopwatch was        */
/*  started, but does not stop or deallocate the stopwatch.                   */
SZMQ_EXPORT unsigned long zmq_stopwatch_intermediate (void *watch_);

/*  Stops the stopwatch. Returns the number of microseconds elapsed since     */
/*  the stopwatch was started, and deallocates that watch.                    */
SZMQ_EXPORT unsigned long zmq_stopwatch_stop (void *watch_);

/*  Sleeps for specified number of seconds.                                   */
SZMQ_EXPORT void zmq_sleep (int seconds_);

typedef void (zmq_thread_fn) (void *);

/* Start a thread. Returns a handle to the thread.                            */
SZMQ_EXPORT void *zmq_threadstart (zmq_thread_fn *func_, void *arg_);

/* Wait for thread to complete then free up resources.                        */
SZMQ_EXPORT void zmq_threadclose (void *thread_);


/******************************************************************************/
/*  These functions are DRAFT and disabled in stable releases, and subject to */
/*  change at ANY time until declared stable.                                 */
/******************************************************************************/

#ifdef SZMQ_BUILD_DRAFT_API

/*  DRAFT Socket types.                                                       */
#define SZMQ_SERVER 12
#define SZMQ_CLIENT 13
#define SZMQ_RADIO 14
#define SZMQ_DISH 15
#define SZMQ_GATHER 16
#define SZMQ_SCATTER 17
#define SZMQ_DGRAM 18
#define SZMQ_PEER 19
#define SZMQ_CHANNEL 20

/*  DRAFT Socket options.                                                     */
#define SZMQ_ZAP_ENFORCE_DOMAIN 93
#define SZMQ_LOOPBACK_FASTPATH 94
#define SZMQ_METADATA 95
#define SZMQ_MULTICAST_LOOP 96
#define SZMQ_ROUTER_NOTIFY 97
#define SZMQ_XPUB_MANUAL_LAST_VALUE 98
#define SZMQ_SOCKS_USERNAME 99
#define SZMQ_SOCKS_PASSWORD 100
#define SZMQ_IN_BATCH_SIZE 101
#define SZMQ_OUT_BATCH_SIZE 102
#define SZMQ_WSS_KEY_PEM 103
#define SZMQ_WSS_CERT_PEM 104
#define SZMQ_WSS_TRUST_PEM 105
#define SZMQ_WSS_HOSTNAME 106
#define SZMQ_WSS_TRUST_SYSTEM 107
#define SZMQ_ONLY_FIRST_SUBSCRIBE 108
#define SZMQ_RECONNECT_STOP 109
#define SZMQ_HELLO_MSG 110
#define SZMQ_DISCONNECT_MSG 111
#define SZMQ_PRIORITY 112
#define SZMQ_BUSY_POLL 113
#define SZMQ_HICCUP_MSG 114
#define SZMQ_XSUB_VERBOSE_UNSUBSCRIBE 115

/*  DRAFT SZMQ_RECONNECT_STOP options                                          */
#define SZMQ_RECONNECT_STOP_CONN_REFUSED 0x1
#define SZMQ_RECONNECT_STOP_HANDSHAKE_FAILED 0x2
#define SZMQ_RECONNECT_STOP_AFTER_DISCONNECT 0x4

/*  DRAFT Context options                                                     */
#define SZMQ_ZERO_COPY_RECV 10

/*  DRAFT Context methods.                                                    */
SZMQ_EXPORT int zmq_ctx_set_ext (void *context_,
                                int option_,
                                const void *optval_,
                                size_t optvallen_);
SZMQ_EXPORT int zmq_ctx_get_ext (void *context_,
                                int option_,
                                void *optval_,
                                size_t *optvallen_);

/*  DRAFT Socket methods.                                                     */
SZMQ_EXPORT int zmq_join (void *s, const char *group);
SZMQ_EXPORT int zmq_leave (void *s, const char *group);
SZMQ_EXPORT uint32_t zmq_connect_peer (void *s_, const char *addr_);

/*  DRAFT Msg methods.                                                        */
SZMQ_EXPORT int zmq_msg_set_routing_id (zmq_msg_t *msg, uint32_t routing_id);
SZMQ_EXPORT uint32_t zmq_msg_routing_id (zmq_msg_t *msg);
SZMQ_EXPORT int zmq_msg_set_group (zmq_msg_t *msg, const char *group);
SZMQ_EXPORT const char *zmq_msg_group (zmq_msg_t *msg);
SZMQ_EXPORT int
zmq_msg_init_buffer (zmq_msg_t *msg_, const void *buf_, size_t size_);

/*  DRAFT Msg property names.                                                 */
#define SZMQ_MSG_PROPERTY_ROUTING_ID "Routing-Id"
#define SZMQ_MSG_PROPERTY_SOCKET_TYPE "Socket-Type"
#define SZMQ_MSG_PROPERTY_USER_ID "User-Id"
#define SZMQ_MSG_PROPERTY_PEER_ADDRESS "Peer-Address"

/*  Router notify options                                                     */
#define SZMQ_NOTIFY_CONNECT 1
#define SZMQ_NOTIFY_DISCONNECT 2

/******************************************************************************/
/*  Poller polling on sockets,fd and thread-safe sockets                      */
/******************************************************************************/

#define SZMQ_HAVE_POLLER

typedef struct zmq_poller_event_t
{
    void *socket;
    zmq_fd_t fd;
    void *user_data;
    short events;
} zmq_poller_event_t;

SZMQ_EXPORT void *zmq_poller_new (void);
SZMQ_EXPORT int zmq_poller_destroy (void **poller_p);
SZMQ_EXPORT int zmq_poller_size (void *poller);
SZMQ_EXPORT int
zmq_poller_add (void *poller, void *socket, void *user_data, short events);
SZMQ_EXPORT int zmq_poller_modify (void *poller, void *socket, short events);
SZMQ_EXPORT int zmq_poller_remove (void *poller, void *socket);
SZMQ_EXPORT int
zmq_poller_wait (void *poller, zmq_poller_event_t *event, long timeout);
SZMQ_EXPORT int zmq_poller_wait_all (void *poller,
                                    zmq_poller_event_t *events,
                                    int n_events,
                                    long timeout);
SZMQ_EXPORT int zmq_poller_fd (void *poller, zmq_fd_t *fd);

SZMQ_EXPORT int
zmq_poller_add_fd (void *poller, zmq_fd_t fd, void *user_data, short events);
SZMQ_EXPORT int zmq_poller_modify_fd (void *poller, zmq_fd_t fd, short events);
SZMQ_EXPORT int zmq_poller_remove_fd (void *poller, zmq_fd_t fd);

SZMQ_EXPORT int zmq_socket_get_peer_state (void *socket,
                                          const void *routing_id,
                                          size_t routing_id_size);

/*  DRAFT Socket monitoring events                                            */
#define SZMQ_EVENT_PIPES_STATS 0x10000

#define SZMQ_CURRENT_EVENT_VERSION 1
#define SZMQ_CURRENT_EVENT_VERSION_DRAFT 2

#define SZMQ_EVENT_ALL_V1 SZMQ_EVENT_ALL
#define SZMQ_EVENT_ALL_V2 SZMQ_EVENT_ALL_V1 | SZMQ_EVENT_PIPES_STATS

SZMQ_EXPORT int zmq_socket_monitor_versioned (
  void *s_, const char *addr_, uint64_t events_, int event_version_, int type_);
SZMQ_EXPORT int zmq_socket_monitor_pipes_stats (void *s);

#if !defined _WIN32
SZMQ_EXPORT int zmq_ppoll (zmq_pollitem_t *items_,
                          int nitems_,
                          long timeout_,
                          const sigset_t *sigmask_);
#else
// Windows has no sigset_t
SZMQ_EXPORT int zmq_ppoll (zmq_pollitem_t *items_,
                          int nitems_,
                          long timeout_,
                          const void *sigmask_);
#endif

#endif // SZMQ_BUILD_DRAFT_API


#undef SZMQ_EXPORT

#ifdef __cplusplus
}
#endif

#endif
