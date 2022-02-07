

#ifndef __SZMQ_REP_HPP_INCLUDED__
#define __SZMQ_REP_HPP_INCLUDED__

#include "router.hpp"

namespace zmq {
    class ctx_t;

    class msg_t;

    class io_thread_t;

    class socket_base_t;

    class rep_t SZMQ_FINAL : public router_t {
    public:
        rep_t(zmq::ctx_t *parent_, uint32_t tid_, int sid_);

        ~rep_t();

        //  Overrides of functions from socket_base_t.
        int xsend(zmq::msg_t *msg_);

        int xrecv(zmq::msg_t *msg_);

        bool xhas_in();

        bool xhas_out();

    private:
        //  If true, we are in process of sending the reply. If false we are
        //  in process of receiving a request.
        bool _sending_reply;

        //  If true, we are starting to receive a request. The beginning
        //  of the request is the backtrace stack.
        bool _request_begins;

    SZMQ_NON_COPYABLE_NOR_MOVABLE (rep_t)
    };
}

#endif
