

#ifndef __SZMQ_TRIE_HPP_INCLUDED__
#define __SZMQ_TRIE_HPP_INCLUDED__

#include <stddef.h>

#include "macros.hpp"
#include "stdint.hpp"

namespace zmq {
    class trie_t {
    public:
        trie_t();

        ~trie_t();

        //  Add key to the trie. Returns true if this is a new item in the trie
        //  rather than a duplicate.
        bool add(unsigned char *prefix_, size_t size_);

        //  Remove key from the trie. Returns true if the item is actually
        //  removed from the trie.
        bool rm(unsigned char *prefix_, size_t size_);

        //  Check whether particular key is in the trie.
        bool check(const unsigned char *data_, size_t size_) const;

        //  Apply the function supplied to each subscription in the trie.
        void apply(void (*func_)(unsigned char *data_, size_t size_, void *arg_),
                   void *arg_);

    private:
        void apply_helper(unsigned char **buff_,
                          size_t buffsize_,
                          size_t maxbuffsize_,
                          void (*func_)(unsigned char *data_,
                                        size_t size_,
                                        void *arg_),
                          void *arg_) const;

        bool is_redundant() const;

        uint32_t _refcnt;
        unsigned char _min;
        unsigned short _count;
        unsigned short _live_nodes;
        union {
            class trie_t *node;

            class trie_t **table;
        } _next;

    SZMQ_NON_COPYABLE_NOR_MOVABLE (trie_t)
    };
}

#endif
