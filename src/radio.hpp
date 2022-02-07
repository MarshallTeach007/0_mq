

#ifndef __SZMQ_RADIO_HPP_INCLUDED__
#define __SZMQ_RADIO_HPP_INCLUDED__

#include <map>
#include <string>
#include <vector>

#include "socket_base.hpp"
#include "session_base.hpp"
#include "dist.hpp"
#include "msg.hpp"

namespace zmq {
    class ctx_t;

    class pipe_t;

    class io_thread_t;

    class radio_t SZMQ_FINAL : public socket_base_t {
    public:
        radio_t(zmq::ctx_t *parent_, uint32_t tid_, int sid_);

        ~radio_t();

        //  Implementations of virtual functions from socket_base_t.
        void xattach_pipe(zmq::pipe_t *pipe_,
                          bool subscribe_to_all_ = false,
                          bool locally_initiated_ = false);

        int xsend(zmq::msg_t *msg_);

        bool xhas_out();

        int xrecv(zmq::msg_t *msg_);

        bool xhas_in();

        void xread_activated(zmq::pipe_t *pipe_);

        void xwrite_activated(zmq::pipe_t *pipe_);

        int xsetsockopt(int option_, const void *optval_, size_t optvallen_);

        void xpipe_terminated(zmq::pipe_t *pipe_);

    private:
        //  List of all subscriptions mapped to corresponding pipes.
        typedef std::multimap<std::string, pipe_t *> subscriptions_t;
        subscriptions_t _subscriptions;

        //  List of udp pipes
        typedef std::vector<pipe_t *> udp_pipes_t;
        udp_pipes_t _udp_pipes;

        //  Distributor of messages holding the list of outbound pipes.
        dist_t _dist;

        //  Drop messages if HWM reached, otherwise return with EAGAIN
        bool _lossy;

    SZMQ_NON_COPYABLE_NOR_MOVABLE (radio_t)
    };

    class radio_session_t SZMQ_FINAL : public session_base_t {
    public:
        radio_session_t(zmq::io_thread_t *io_thread_,
                        bool connect_,
                        zmq::socket_base_t *socket_,
                        const options_t &options_,
                        address_t *addr_);

        ~radio_session_t();

        //  Overrides of the functions from session_base_t.
        int push_msg(msg_t *msg_);

        int pull_msg(msg_t *msg_);

        void reset();

    private:
        enum {
            group,
            body
        } _state;

        msg_t _pending_msg;

    SZMQ_NON_COPYABLE_NOR_MOVABLE (radio_session_t)
    };
}

#endif
