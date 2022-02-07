

#include <string>
#include <sstream>
#include <vmci_sockets.h>

#include "testutil.hpp"
#include "testutil_unity.hpp"

SETUP_TEARDOWN_TESTCONTEXT

void test_pair_vmci ()
{
    std::stringstream s;
    s << "vmci://" << VMCISock_GetLocalCID () << ":" << 5560;
    std::string endpoint = s.str ();

    void *sb = test_context_socket (SZMQ_PAIR);
    int rc = zmq_bind (sb, endpoint.c_str ());
    if (rc < 0 && errno == EAFNOSUPPORT)
        TEST_IGNORE_MESSAGE ("VMCI not supported");
    TEST_ASSERT_SUCCESS_ERRNO (rc);

    void *sc = test_context_socket (SZMQ_PAIR);
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (sc, endpoint.c_str ()));

    expect_bounce_fail (sb, sc);

    test_context_socket_close_zero_linger (sc);
    test_context_socket_close_zero_linger (sb);
}

int main (void)
{
    setup_test_environment ();

    UNITY_BEGIN ();
    RUN_TEST (test_pair_vmci);
    return UNITY_END ();
}
