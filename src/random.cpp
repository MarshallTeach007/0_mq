

#include "precompiled.hpp"
#include <stdlib.h>

#if !defined SZMQ_HAVE_WINDOWS

#include <unistd.h>

#endif

#include "random.hpp"
#include "stdint.hpp"
#include "clock.hpp"
#include "mutex.hpp"
#include "macros.hpp"

#if defined(SZMQ_USE_TWEETNACL)
#include "tweetnacl.h"
#elif defined(SZMQ_USE_LIBSODIUM)
#include "sodium.h"
#endif

void zmq::seed_random() {
#if defined SZMQ_HAVE_WINDOWS
    const int pid = static_cast<int> (GetCurrentProcessId ());
#else
    int pid = static_cast<int> (getpid());
#endif
    srand(static_cast<unsigned int> (clock_t::now_us() + pid));
}

uint32_t zmq::generate_random() {
    //  Compensate for the fact that rand() returns signed integer.
    const uint32_t low = static_cast<uint32_t> (rand());
    uint32_t high = static_cast<uint32_t> (rand());
    high <<= (sizeof(int) * 8 - 1);
    return high | low;
}

//  When different threads have their own context the file descriptor
//  variable is shared and is subject to race conditions in tweetnacl,
//  that lead to file descriptors leaks. In long-running programs with
//  ephemeral threads this is a problem as it accumulates.
//  thread-local storage cannot be used to initialise the file descriptor
//  as it is perfectly legal to share a context among many threads, each
//  of which might call curve APIs.
//  Also libsodium documentation specifically states that sodium_init
//  must not be called concurrently from multiple threads, for the
//  same reason. Inspecting the code also reveals that the close API is
//  not thread safe.
//  The context class cannot be used with static variables as the curve
//  utility APIs like zmq_curve_keypair also call into the crypto
//  library.
//  The safest solution for all use cases therefore is to have a
//  static lock to serialize calls into an initialiser and a finaliser,
//  using refcounts to make sure that a thread does not close the library
//  while another is still using it. To avoid the static initialization
//  order fiasco, this is done using function-local statics, if the
//  compiler implementation supports thread-safe initialization of those.
//  Otherwise, we fall back to global statics.
//  HOWEVER, this initialisation code imposes ordering constraints, which
//  are not obvious to users of libzmq, and may lead to problems if atexit
//  or similar methods are used for cleanup.
//  In that case, a strict ordering is imposed whereas the contexts MUST
//  be initialised BEFORE registering the cleanup with atexit. CZMQ is an
//  example. Hence we make the choice to restrict this global transition
//  mechanism ONLY to Tweenacl + *NIX (when using /dev/urandom) as it is
//  the less risky option.

//  TODO if there is some other user of libsodium besides libzmq, this must
//    be synchronized by the application. This should probably also be
//    configurable via config.h

//  TODO this should probably be done via config.h
#if __cplusplus >= 201103L                                                     \
 || (defined(__cpp_threadsafe_static_init)                                    \
 && __cpp_threadsafe_static_init >= 200806)                               \
 || (defined(_MSC_VER) && _MSC_VER >= 1900)
#define SZMQ_HAVE_THREADSAFE_STATIC_LOCAL_INIT 1
//  TODO this might probably also be set if a sufficiently recent gcc is used
//  without -fno-threadsafe-statics, but this cannot be determined at
//  compile-time, so it must be set via config.h
#else
#define SZMQ_HAVE_THREADSAFE_STATIC_LOCAL_INIT 0
#endif

#if !SZMQ_HAVE_THREADSAFE_STATIC_LOCAL_INIT                                     \
 && (defined(SZMQ_USE_TWEETNACL) && !defined(SZMQ_HAVE_WINDOWS)                 \
 && !defined(SZMQ_HAVE_GETRANDOM))
static unsigned int random_refcount = 0;
static zmq::mutex_t random_sync;
#endif

static void manage_random(bool init_) {
#if defined(SZMQ_USE_TWEETNACL) && !defined(SZMQ_HAVE_WINDOWS)                   \
 && !defined(SZMQ_HAVE_GETRANDOM)

#if SZMQ_HAVE_THREADSAFE_STATIC_LOCAL_INIT
    static int random_refcount = 0;
    static zmq::mutex_t random_sync;
#endif

    if (init_) {
        zmq::scoped_lock_t locker (random_sync);

        if (random_refcount == 0) {
            int rc = sodium_init ();
            zmq_assert (rc != -1);
        }

        ++random_refcount;
    } else {
        zmq::scoped_lock_t locker (random_sync);
        --random_refcount;

        if (random_refcount == 0) {
            randombytes_close ();
        }
    }

#elif defined(SZMQ_USE_LIBSODIUM)
    if (init_) {
        int rc = sodium_init ();
        zmq_assert (rc != -1);
#if defined(SZMQ_LIBSODIUM_RANDOMBYTES_CLOSE)
    } else {
        // randombytes_close either a no-op or not threadsafe
        // doing this without refcounting can cause crashes
        // if called while a context is active
        randombytes_close ();
#endif
    }
#else
    LIBSZMQ_UNUSED (init_);
#endif
}

void zmq::random_open() {
    manage_random(true);
}

void zmq::random_close() {
    manage_random(false);
}