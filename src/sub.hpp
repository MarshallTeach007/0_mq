

#ifndef __SZMQ_SUB_HPP_INCLUDED__
#define __SZMQ_SUB_HPP_INCLUDED__

#include "xsub.hpp"

namespace zmq {
    class ctx_t;

    class msg_t;

    class io_thread_t;

    class socket_base_t;

    class sub_t SZMQ_FINAL : public xsub_t {
    public:
        sub_t(zmq::ctx_t *parent_, uint32_t tid_, int sid_);

        ~sub_t();

    protected:
        int xsetsockopt(int option_, const void *optval_, size_t optvallen_);

        int xsend(zmq::msg_t *msg_);

        bool xhas_out();

    SZMQ_NON_COPYABLE_NOR_MOVABLE (sub_t)
    };
}

#endif
