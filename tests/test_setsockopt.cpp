

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <string.h>

SETUP_TEARDOWN_TESTCONTEXT

void test_setsockopt_tcp_recv_buffer ()
{
    void *socket = test_context_socket (SZMQ_PUSH);

    int val = 0;
    size_t placeholder = sizeof (val);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_RCVBUF, &val, &placeholder));
    TEST_ASSERT_EQUAL_INT (-1, val);

    val = 16384;

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (socket, SZMQ_RCVBUF, &val, sizeof (val)));
    TEST_ASSERT_EQUAL_INT (16384, val);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_RCVBUF, &val, &placeholder));
    TEST_ASSERT_EQUAL_INT (16384, val);

    test_context_socket_close (socket);
}

void test_setsockopt_tcp_send_buffer ()
{
    void *socket = test_context_socket (SZMQ_PUSH);

    int val = 0;
    size_t placeholder = sizeof (val);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_SNDBUF, &val, &placeholder));
    TEST_ASSERT_EQUAL_INT (-1, val);

    val = 16384;

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (socket, SZMQ_SNDBUF, &val, sizeof (val)));
    TEST_ASSERT_EQUAL_INT (16384, val);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_SNDBUF, &val, &placeholder));
    TEST_ASSERT_EQUAL_INT (16384, val);

    test_context_socket_close (socket);
}

void test_setsockopt_use_fd ()
{
    void *socket = test_context_socket (SZMQ_PUSH);

    int val = 0;
    size_t placeholder = sizeof (val);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_USE_FD, &val, &placeholder));
    TEST_ASSERT_EQUAL_INT (-1, val);

    val = 3;

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (socket, SZMQ_USE_FD, &val, sizeof (val)));
    TEST_ASSERT_EQUAL_INT (3, val);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_USE_FD, &val, &placeholder));
    TEST_ASSERT_EQUAL_INT (3, val);

    test_context_socket_close (socket);
}

#define BOUNDDEVBUFSZ 16
void test_setsockopt_bindtodevice ()
{
    void *socket = test_context_socket (SZMQ_PUSH);

#ifdef SZMQ_BINDTODEVICE
    char devname[BOUNDDEVBUFSZ];
    size_t buflen = BOUNDDEVBUFSZ;

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_BINDTODEVICE, devname, &buflen));
    TEST_ASSERT_EQUAL_INT8 ('\0', devname[0]);
    TEST_ASSERT_EQUAL_UINT (1, buflen);

    sprintf (devname, "testdev");
    buflen = strlen (devname);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (socket, SZMQ_BINDTODEVICE, devname, buflen));

    buflen = BOUNDDEVBUFSZ;
    memset (devname, 0, buflen);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_BINDTODEVICE, devname, &buflen));
    TEST_ASSERT_EQUAL_STRING_LEN ("testdev", devname, buflen);
#endif

    test_context_socket_close (socket);
}

void test_setsockopt_priority ()
{
#ifdef SZMQ_BUILD_DRAFT_API
#ifdef SZMQ_HAVE_SO_PRIORITY
    void *socket = test_context_socket (SZMQ_PUSH);

    int val = 5;
    size_t placeholder = sizeof (val);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_PRIORITY, &val, &placeholder));
    TEST_ASSERT_EQUAL_INT (0, val);

    val = 3;

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (socket, SZMQ_PRIORITY, &val, sizeof (val)));
    TEST_ASSERT_EQUAL_INT (3, val);

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt (socket, SZMQ_PRIORITY, &val, &placeholder));
    TEST_ASSERT_EQUAL_INT (3, val);

    test_context_socket_close (socket);
#else
    TEST_IGNORE_MESSAGE ("libzmq without SZMQ_PRIORITY support, "
                         "ignoring setsockopt_priority test");
#endif
#else
    TEST_IGNORE_MESSAGE ("libzmq without DRAFT support, ignoring "
                         "setsockopt_priority test");
#endif
}

int main ()
{
    setup_test_environment ();

    UNITY_BEGIN ();
    RUN_TEST (test_setsockopt_tcp_recv_buffer);
    RUN_TEST (test_setsockopt_tcp_send_buffer);
    RUN_TEST (test_setsockopt_use_fd);
    RUN_TEST (test_setsockopt_bindtodevice);
    RUN_TEST (test_setsockopt_priority);
    return UNITY_END ();
}
