

#ifndef __SZMQ_TIPC_ADDRESS_HPP_INCLUDED__
#define __SZMQ_TIPC_ADDRESS_HPP_INCLUDED__

#include <string>

#include "platform.hpp"

#if defined SZMQ_HAVE_TIPC

#include <sys/socket.h>
#if defined SZMQ_HAVE_VXWORKS
#include <tipc/tipc.h>
#else
#include <linux/tipc.h>
#endif

namespace zmq
{
class tipc_address_t
{
  public:
    tipc_address_t ();
    tipc_address_t (const sockaddr *sa, socklen_t sa_len);

    //  This function sets up the address "{type, lower, upper}" for TIPC transport
    int resolve (const char *name);

    //  The opposite to resolve()
    int to_string (std::string &addr_) const;

    // Handling different TIPC address types
    bool is_service () const;
    bool is_random () const;
    void set_random ();

    const sockaddr *addr () const;
    socklen_t addrlen () const;

  private:
    bool _random;
    struct sockaddr_tipc address;
};
}

#endif

#endif
