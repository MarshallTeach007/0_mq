

#ifndef __SZMQ_XSUB_HPP_INCLUDED__
#define __SZMQ_XSUB_HPP_INCLUDED__

#include "socket_base.hpp"
#include "session_base.hpp"
#include "dist.hpp"
#include "fq.hpp"

#ifdef SZMQ_USE_RADIX_TREE
#include "radix_tree.hpp"
#else

#include "trie.hpp"

#endif

namespace zmq {
    class ctx_t;

    class pipe_t;

    class io_thread_t;

    class xsub_t : public socket_base_t {
    public:
        xsub_t(zmq::ctx_t *parent_, uint32_t tid_, int sid_);

        ~xsub_t() SZMQ_OVERRIDE;

    protected:
        //  Overrides of functions from socket_base_t.
        void xattach_pipe(zmq::pipe_t *pipe_,
                          bool subscribe_to_all_,
                          bool locally_initiated_) SZMQ_FINAL;

        int xsetsockopt(int option_,
                        const void *optval_,
                        size_t optvallen_) SZMQ_OVERRIDE;

        int xsend(zmq::msg_t *msg_) SZMQ_OVERRIDE;

        bool xhas_out() SZMQ_OVERRIDE;

        int xrecv(zmq::msg_t *msg_) SZMQ_FINAL;

        bool xhas_in() SZMQ_FINAL;

        void xread_activated(zmq::pipe_t *pipe_) SZMQ_FINAL;

        void xwrite_activated(zmq::pipe_t *pipe_) SZMQ_FINAL;

        void xhiccuped(pipe_t *pipe_) SZMQ_FINAL;

        void xpipe_terminated(zmq::pipe_t *pipe_) SZMQ_FINAL;

    private:
        //  Check whether the message matches at least one subscription.
        bool match(zmq::msg_t *msg_);

        //  Function to be applied to the trie to send all the subsciptions
        //  upstream.
        static void
        send_subscription(unsigned char *data_, size_t size_, void *arg_);

        //  Fair queueing object for inbound pipes.
        fq_t _fq;

        //  Object for distributing the subscriptions upstream.
        dist_t _dist;

        //  The repository of subscriptions.
#ifdef SZMQ_USE_RADIX_TREE
        radix_tree_t _subscriptions;
#else
        trie_t _subscriptions;
#endif

        // If true, send all unsubscription messages upstream, not just
        // unique ones
        bool _verbose_unsubs;

        //  If true, 'message' contains a matching message to return on the
        //  next recv call.
        bool _has_message;
        msg_t _message;

        //  If true, part of a multipart message was already sent, but
        //  there are following parts still waiting.
        bool _more_send;

        //  If true, part of a multipart message was already received, but
        //  there are following parts still waiting.
        bool _more_recv;

        //  If true, subscribe and cancel messages are processed for the rest
        //  of multipart message.
        bool _process_subscribe;

        //  This option is enabled with SZMQ_ONLY_FIRST_SUBSCRIBE.
        //  If true, messages following subscribe/unsubscribe in a multipart
        //  message are treated as user data regardless of the first byte.
        bool _only_first_subscribe;

    SZMQ_NON_COPYABLE_NOR_MOVABLE (xsub_t)
    };
}

#endif
