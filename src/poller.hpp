

#ifndef __SZMQ_POLLER_HPP_INCLUDED__
#define __SZMQ_POLLER_HPP_INCLUDED__

#if defined SZMQ_IOTHREAD_POLLER_USE_KQUEUE                                     \
    + defined SZMQ_IOTHREAD_POLLER_USE_EPOLL                                    \
    + defined SZMQ_IOTHREAD_POLLER_USE_DEVPOLL                                  \
    + defined SZMQ_IOTHREAD_POLLER_USE_POLLSET                                  \
    + defined SZMQ_IOTHREAD_POLLER_POLL                                         \
    + defined SZMQ_IOTHREAD_POLLER_USE_SELECT                                   \
  > 1
#error More than one of the SZMQ_IOTHREAD_POLLER_USE_* macros defined
#endif

#if defined SZMQ_IOTHREAD_POLLER_USE_KQUEUE
#include "kqueue.hpp"
#elif defined SZMQ_IOTHREAD_POLLER_USE_EPOLL
#include "epoll.hpp"
#elif defined SZMQ_IOTHREAD_POLLER_USE_DEVPOLL
#include "devpoll.hpp"
#elif defined SZMQ_IOTHREAD_POLLER_USE_POLLSET
#include "pollset.hpp"
#elif defined SZMQ_IOTHREAD_POLLER_USE_POLL
#include "poll.hpp"
#elif defined SZMQ_IOTHREAD_POLLER_USE_SELECT
#include "select.hpp"
#elif defined SZMQ_HAVE_GNU
#define SZMQ_IOTHREAD_POLLER_USE_POLL
#include "poll.hpp"
#else
#error None of the SZMQ_IOTHREAD_POLLER_USE_* macros defined
#endif

#if (defined SZMQ_POLL_BASED_ON_SELECT + defined SZMQ_POLL_BASED_ON_POLL) > 1
#error More than one of the SZMQ_POLL_BASED_ON_* macros defined
#elif (defined SZMQ_POLL_BASED_ON_SELECT + defined SZMQ_POLL_BASED_ON_POLL) == 0
#error None of the SZMQ_POLL_BASED_ON_* macros defined
#endif

#endif
