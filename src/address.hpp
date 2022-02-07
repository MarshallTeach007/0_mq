

#ifndef __SZMQ_ADDRESS_HPP_INCLUDED__
#define __SZMQ_ADDRESS_HPP_INCLUDED__

#include "fd.hpp"

#include <string>

#ifndef SZMQ_HAVE_WINDOWS

#include <sys/socket.h>

#else
#include <ws2tcpip.h>
#endif

namespace zmq {
    class ctx_t;

    class tcp_address_t;

    class udp_address_t;

    class ws_address_t;

#ifdef SZMQ_HAVE_WSS
    class wss_address_t;
#endif
#if defined SZMQ_HAVE_IPC
    class ipc_address_t;
#endif
#if defined SZMQ_HAVE_LINUX || defined SZMQ_HAVE_VXWORKS
    class tipc_address_t;
#endif
#if defined SZMQ_HAVE_VMCI
    class vmci_address_t;
#endif

    namespace protocol_name {
        static const char inproc[] = "inproc";
        static const char tcp[] = "tcp";
        static const char udp[] = "udp";
#ifdef SZMQ_HAVE_OPENPGM
        static const char pgm[] = "pgm";
        static const char epgm[] = "epgm";
#endif
#ifdef SZMQ_HAVE_NORM
        static const char norm[] = "norm";
#endif
#ifdef SZMQ_HAVE_WS
        static const char ws[] = "ws";
#endif
#ifdef SZMQ_HAVE_WSS
        static const char wss[] = "wss";
#endif
#if defined SZMQ_HAVE_IPC
        static const char ipc[] = "ipc";
#endif
#if defined SZMQ_HAVE_TIPC
        static const char tipc[] = "tipc";
#endif
#if defined SZMQ_HAVE_VMCI
        static const char vmci[] = "vmci";
#endif
    }

    struct address_t {
        address_t(const std::string &protocol_,
                  const std::string &address_,
                  ctx_t *parent_);

        ~address_t();

        const std::string protocol;
        const std::string address;
        ctx_t *const parent;

        //  Protocol specific resolved address
        //  All members must be pointers to allow for consistent initialization
        union {
            void *dummy;
            tcp_address_t *tcp_addr;
            udp_address_t *udp_addr;
#ifdef SZMQ_HAVE_WS
            ws_address_t *ws_addr;
#endif
#ifdef SZMQ_HAVE_WSS
            wss_address_t *wss_addr;
#endif
#if defined SZMQ_HAVE_IPC
            ipc_address_t *ipc_addr;
#endif
#if defined SZMQ_HAVE_LINUX || defined SZMQ_HAVE_VXWORKS
            tipc_address_t *tipc_addr;
#endif
#if defined SZMQ_HAVE_VMCI
            vmci_address_t *vmci_addr;
#endif
        } resolved;

        int to_string(std::string &addr_) const;
    };

#if defined(SZMQ_HAVE_HPUX) || defined(SZMQ_HAVE_VXWORKS)                        \
 || defined(SZMQ_HAVE_WINDOWS)
    typedef int zmq_socklen_t;
#else
    typedef socklen_t zmq_socklen_t;
#endif

    enum socket_end_t {
        socket_end_local,
        socket_end_remote
    };

    zmq_socklen_t
    get_socket_address(fd_t fd_, socket_end_t socket_end_, sockaddr_storage *ss_);

    template<typename T>
    std::string get_socket_name(fd_t fd_, socket_end_t socket_end_) {
        struct sockaddr_storage ss;
        const zmq_socklen_t sl = get_socket_address(fd_, socket_end_, &ss);
        if (sl == 0) {
            return std::string();
        }

        const T addr(reinterpret_cast<struct sockaddr *> (&ss), sl);
        std::string address_string;
        addr.to_string(address_string);
        return address_string;
    }
}

#endif
