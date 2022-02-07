

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <string.h>

SETUP_TEARDOWN_TESTCONTEXT

/// Initialize a zeromq message with a given null-terminated string
#define SZMQ_PREPARE_STRING(msg, data, size)                                    \
    TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_init (&msg));                           \
    TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_init_size (&msg, size + 1));            \
    memcpy (zmq_msg_data (&msg), data, size + 1);

static int publicationsReceived = 0;
static bool isSubscribed = false;

void test_disconnect_inproc ()
{
    void *pub_socket = test_context_socket (SZMQ_XPUB);
    void *sub_socket = test_context_socket (SZMQ_SUB);
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (sub_socket, SZMQ_SUBSCRIBE, "foo", 3));

    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_bind (pub_socket, "inproc://someInProcDescriptor"));

    int more;
    size_t more_size = sizeof (more);

    for (int iteration = 0;; ++iteration) {
        zmq_pollitem_t items[] = {
          {sub_socket, 0, SZMQ_POLLIN, 0}, // read publications
          {pub_socket, 0, SZMQ_POLLIN, 0}, // read subscriptions
        };
        int rc = zmq_poll (items, 2, 100);

        if (items[1].revents & SZMQ_POLLIN) {
            for (more = 1; more;) {
                zmq_msg_t msg;
                zmq_msg_init (&msg);
                TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_recv (&msg, pub_socket, 0));
                const char *const buffer =
                  static_cast<const char *> (zmq_msg_data (&msg));

                if (buffer[0] == 0) {
                    TEST_ASSERT_TRUE (isSubscribed);
                    isSubscribed = false;
                } else {
                    TEST_ASSERT_FALSE (isSubscribed);
                    isSubscribed = true;
                }

                TEST_ASSERT_SUCCESS_ERRNO (
                  zmq_getsockopt (pub_socket, SZMQ_RCVMORE, &more, &more_size));
                TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_close (&msg));
            }
        }

        if (items[0].revents & SZMQ_POLLIN) {
            more = 1;
            for (more = 1; more;) {
                zmq_msg_t msg;
                TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_init (&msg));
                TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_recv (&msg, sub_socket, 0));
                TEST_ASSERT_SUCCESS_ERRNO (
                  zmq_getsockopt (sub_socket, SZMQ_RCVMORE, &more, &more_size));
                TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_close (&msg));
            }
            publicationsReceived++;
        }
        if (iteration == 1) {
            TEST_ASSERT_SUCCESS_ERRNO (
              zmq_connect (sub_socket, "inproc://someInProcDescriptor"));
            msleep (SETTLE_TIME);
        }
        if (iteration == 4) {
            TEST_ASSERT_SUCCESS_ERRNO (
              zmq_disconnect (sub_socket, "inproc://someInProcDescriptor"));
        }
        if (iteration > 4 && rc == 0)
            break;

        zmq_msg_t channel_envlp;
        SZMQ_PREPARE_STRING (channel_envlp, "foo", 3);
        TEST_ASSERT_SUCCESS_ERRNO (
          zmq_msg_send (&channel_envlp, pub_socket, SZMQ_SNDMORE));
        TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_close (&channel_envlp));

        zmq_msg_t message;
        SZMQ_PREPARE_STRING (message, "this is foo!", 12);
        TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_send (&message, pub_socket, 0));
        TEST_ASSERT_SUCCESS_ERRNO (zmq_msg_close (&message));
    }
    TEST_ASSERT_EQUAL_INT (3, publicationsReceived);
    TEST_ASSERT_FALSE (isSubscribed);

    test_context_socket_close (pub_socket);
    test_context_socket_close (sub_socket);
}

int main (int, char **)
{
    setup_test_environment ();

    UNITY_BEGIN ();
    RUN_TEST (test_disconnect_inproc);
    return UNITY_END ();
}
