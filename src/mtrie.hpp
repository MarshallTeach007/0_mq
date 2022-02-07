

#ifndef __SZMQ_MTRIE_HPP_INCLUDED__
#define __SZMQ_MTRIE_HPP_INCLUDED__

#include "generic_mtrie.hpp"

#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER > 1600)
#define SZMQ_HAS_EXTERN_TEMPLATE 1
#else
#define SZMQ_HAS_EXTERN_TEMPLATE 0
#endif

namespace zmq {
    class pipe_t;

#if SZMQ_HAS_EXTERN_TEMPLATE
    extern template class generic_mtrie_t<pipe_t>;
#endif

    typedef generic_mtrie_t<pipe_t> mtrie_t;
}

#endif
