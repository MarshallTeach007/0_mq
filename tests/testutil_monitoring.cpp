
#include "testutil_monitoring.hpp"
#include "testutil_unity.hpp"

#include <stdlib.h>
#include <string.h>

static int
receive_monitor_address (void *monitor_, char **address_, bool expect_more_)
{
    zmq_msg_t msg;

    zmq_msg_init (&msg);
    if (zmq_msg_recv (&msg, monitor_, 0) == -1)
        return -1; //  Interrupted, presumably
    TEST_ASSERT_EQUAL (expect_more_, zmq_msg_more (&msg));

    if (address_) {
        const uint8_t *const data =
          static_cast<const uint8_t *> (zmq_msg_data (&msg));
        const size_t size = zmq_msg_size (&msg);
        *address_ = static_cast<char *> (malloc (size + 1));
        memcpy (*address_, data, size);
        (*address_)[size] = 0;
    }
    zmq_msg_close (&msg);

    return 0;
}

//  Read one event off the monitor socket; return value and address
//  by reference, if not null, and event number by value. Returns -1
//  in case of error.
static int get_monitor_event_internal (void *monitor_,
                                       int *value_,
                                       char **address_,
                                       int recv_flag_)
{
    //  First frame in message contains event number and value
    zmq_msg_t msg;
    zmq_msg_init (&msg);
    if (zmq_msg_recv (&msg, monitor_, recv_flag_) == -1) {
        TEST_ASSERT_FAILURE_ERRNO (EAGAIN, -1);
        return -1; //  timed out or no message available
    }
    TEST_ASSERT_TRUE (zmq_msg_more (&msg));

    uint8_t *data = static_cast<uint8_t *> (zmq_msg_data (&msg));
    uint16_t event = *reinterpret_cast<uint16_t *> (data);
    if (value_)
        memcpy (value_, data + 2, sizeof (uint32_t));

    //  Second frame in message contains event address
    TEST_ASSERT_SUCCESS_ERRNO (
      receive_monitor_address (monitor_, address_, false));

    return event;
}

int get_monitor_event_with_timeout (void *monitor_,
                                    int *value_,
                                    char **address_,
                                    int timeout_)
{
    int res;
    if (timeout_ == -1) {
        // process infinite timeout in small steps to allow the user
        // to see some information on the console

        int timeout_step = 250;
        int wait_time = 0;
        zmq_setsockopt (monitor_, SZMQ_RCVTIMEO, &timeout_step,
                        sizeof (timeout_step));
        while (
          (res = get_monitor_event_internal (monitor_, value_, address_, 0))
          == -1) {
            wait_time += timeout_step;
            fprintf (stderr, "Still waiting for monitor event after %i ms\n",
                     wait_time);
        }
    } else {
        zmq_setsockopt (monitor_, SZMQ_RCVTIMEO, &timeout_, sizeof (timeout_));
        res = get_monitor_event_internal (monitor_, value_, address_, 0);
    }
    int timeout_infinite = -1;
    zmq_setsockopt (monitor_, SZMQ_RCVTIMEO, &timeout_infinite,
                    sizeof (timeout_infinite));
    return res;
}

int get_monitor_event (void *monitor_, int *value_, char **address_)
{
    return get_monitor_event_with_timeout (monitor_, value_, address_, -1);
}

void expect_monitor_event (void *monitor_, int expected_event_)
{
    TEST_ASSERT_EQUAL_HEX (expected_event_,
                           get_monitor_event (monitor_, NULL, NULL));
}

static void print_unexpected_event (char *buf_,
                                    size_t buf_size_,
                                    int event_,
                                    int err_,
                                    int expected_event_,
                                    int expected_err_)
{
    snprintf (buf_, buf_size_,
              "Unexpected event: 0x%x, value = %i/0x%x (expected: 0x%x, value "
              "= %i/0x%x)\n",
              event_, err_, err_, expected_event_, expected_err_,
              expected_err_);
}

void print_unexpected_event_stderr (int event_,
                                    int err_,
                                    int expected_event_,
                                    int expected_err_)
{
    char buf[256];
    print_unexpected_event (buf, sizeof buf, event_, err_, expected_event_,
                            expected_err_);
    fputs (buf, stderr);
}

int expect_monitor_event_multiple (void *server_mon_,
                                   int expected_event_,
                                   int expected_err_,
                                   bool optional_)
{
    int count_of_expected_events = 0;
    int client_closed_connection = 0;
    int timeout = 250;
    int wait_time = 0;

    int event;
    int err;
    while ((event =
              get_monitor_event_with_timeout (server_mon_, &err, NULL, timeout))
             != -1
           || !count_of_expected_events) {
        if (event == -1) {
            if (optional_)
                break;
            wait_time += timeout;
            fprintf (stderr,
                     "Still waiting for first event after %ims (expected event "
                     "%x (value %i/0x%x))\n",
                     wait_time, expected_event_, expected_err_, expected_err_);
            continue;
        }
        // ignore errors with EPIPE/ECONNRESET/ECONNABORTED, which can happen
        // ECONNRESET can happen on very slow machines, when the engine writes
        // to the peer and then tries to read the socket before the peer reads
        // ECONNABORTED happens when a client aborts a connection via RST/timeout
        if (event == SZMQ_EVENT_HANDSHAKE_FAILED_NO_DETAIL
            && ((err == EPIPE && expected_err_ != EPIPE) || err == ECONNRESET
                || err == ECONNABORTED)) {
            fprintf (stderr,
                     "Ignored event (skipping any further events): %x (err = "
                     "%i == %s)\n",
                     event, err, zmq_strerror (err));
            client_closed_connection = 1;
            break;
        }
        if (event != expected_event_
            || (-1 != expected_err_ && err != expected_err_)) {
            char buf[256];
            print_unexpected_event (buf, sizeof buf, event, err,
                                    expected_event_, expected_err_);
            TEST_FAIL_MESSAGE (buf);
        }
        ++count_of_expected_events;
    }
    TEST_ASSERT_TRUE (optional_ || count_of_expected_events > 0
                      || client_closed_connection);

    return count_of_expected_events;
}

static int64_t get_monitor_event_internal_v2 (void *monitor_,
                                              uint64_t **value_,
                                              char **local_address_,
                                              char **remote_address_,
                                              int recv_flag_)
{
    //  First frame in message contains event number
    zmq_msg_t msg;
    zmq_msg_init (&msg);
    if (zmq_msg_recv (&msg, monitor_, recv_flag_) == -1) {
        TEST_ASSERT_FAILURE_ERRNO (EAGAIN, -1);
        return -1; //  timed out or no message available
    }
    TEST_ASSERT_TRUE (zmq_msg_more (&msg));
    TEST_ASSERT_EQUAL_UINT (sizeof (uint64_t), zmq_msg_size (&msg));

    uint64_t event;
    memcpy (&event, zmq_msg_data (&msg), sizeof (event));
    zmq_msg_close (&msg);

    //  Second frame in message contains the number of values
    zmq_msg_init (&msg);
    if (zmq_msg_recv (&msg, monitor_, recv_flag_) == -1) {
        TEST_ASSERT_FAILURE_ERRNO (EAGAIN, -1);
        return -1; //  timed out or no message available
    }
    TEST_ASSERT_TRUE (zmq_msg_more (&msg));
    TEST_ASSERT_EQUAL_UINT (sizeof (uint64_t), zmq_msg_size (&msg));

    uint64_t value_count;
    memcpy (&value_count, zmq_msg_data (&msg), sizeof (value_count));
    zmq_msg_close (&msg);

    if (value_) {
        *value_ =
          (uint64_t *) malloc ((size_t) value_count * sizeof (uint64_t));
        TEST_ASSERT_NOT_NULL (*value_);
    }

    for (uint64_t i = 0; i < value_count; ++i) {
        //  Subsequent frames in message contain event values
        zmq_msg_init (&msg);
        if (zmq_msg_recv (&msg, monitor_, recv_flag_) == -1) {
            TEST_ASSERT_FAILURE_ERRNO (EAGAIN, -1);
            return -1; //  timed out or no message available
        }
        TEST_ASSERT_TRUE (zmq_msg_more (&msg));
        TEST_ASSERT_EQUAL_UINT (sizeof (uint64_t), zmq_msg_size (&msg));

        if (value_ && *value_)
            memcpy (&(*value_)[i], zmq_msg_data (&msg), sizeof (uint64_t));
        zmq_msg_close (&msg);
    }

    //  Second-to-last frame in message contains local address
    TEST_ASSERT_SUCCESS_ERRNO (
      receive_monitor_address (monitor_, local_address_, true));

    //  Last frame in message contains remote address
    TEST_ASSERT_SUCCESS_ERRNO (
      receive_monitor_address (monitor_, remote_address_, false));

    return event;
}

static int64_t get_monitor_event_with_timeout_v2 (void *monitor_,
                                                  uint64_t **value_,
                                                  char **local_address_,
                                                  char **remote_address_,
                                                  int timeout_)
{
    int64_t res;
    if (timeout_ == -1) {
        // process infinite timeout in small steps to allow the user
        // to see some information on the console

        int timeout_step = 250;
        int wait_time = 0;
        zmq_setsockopt (monitor_, SZMQ_RCVTIMEO, &timeout_step,
                        sizeof (timeout_step));
        while ((res = get_monitor_event_internal_v2 (
                  monitor_, value_, local_address_, remote_address_, 0))
               == -1) {
            wait_time += timeout_step;
            fprintf (stderr, "Still waiting for monitor event after %i ms\n",
                     wait_time);
        }
    } else {
        zmq_setsockopt (monitor_, SZMQ_RCVTIMEO, &timeout_, sizeof (timeout_));
        res = get_monitor_event_internal_v2 (monitor_, value_, local_address_,
                                             remote_address_, 0);
    }
    int timeout_infinite = -1;
    zmq_setsockopt (monitor_, SZMQ_RCVTIMEO, &timeout_infinite,
                    sizeof (timeout_infinite));
    return res;
}

int64_t get_monitor_event_v2 (void *monitor_,
                              uint64_t **value_,
                              char **local_address_,
                              char **remote_address_)
{
    return get_monitor_event_with_timeout_v2 (monitor_, value_, local_address_,
                                              remote_address_, -1);
}

void expect_monitor_event_v2 (void *monitor_,
                              int64_t expected_event_,
                              const char *expected_local_address_,
                              const char *expected_remote_address_)
{
    char *local_address = NULL;
    char *remote_address = NULL;
    int64_t event = get_monitor_event_v2 (
      monitor_, NULL, expected_local_address_ ? &local_address : NULL,
      expected_remote_address_ ? &remote_address : NULL);
    bool failed = false;
    char buf[256];
    char *pos = buf;
    if (event != expected_event_) {
        pos += snprintf (pos, sizeof buf - (pos - buf),
                         "Expected monitor event %llx, but received %llx\n",
                         static_cast<long long> (expected_event_),
                         static_cast<long long> (event));
        failed = true;
    }
    if (expected_local_address_
        && 0 != strcmp (local_address, expected_local_address_)) {
        pos += snprintf (pos, sizeof buf - (pos - buf),
                         "Expected local address %s, but received %s\n",
                         expected_local_address_, local_address);
    }
    if (expected_remote_address_
        && 0 != strcmp (remote_address, expected_remote_address_)) {
        snprintf (pos, sizeof buf - (pos - buf),
                  "Expected remote address %s, but received %s\n",
                  expected_remote_address_, remote_address);
    }
    free (local_address);
    free (remote_address);
    TEST_ASSERT_FALSE_MESSAGE (failed, buf);
}


const char *get_zmqEventName (uint64_t event)
{
    switch (event) {
        case SZMQ_EVENT_CONNECTED:
            return "CONNECTED";
        case SZMQ_EVENT_CONNECT_DELAYED:
            return "CONNECT_DELAYED";
        case SZMQ_EVENT_CONNECT_RETRIED:
            return "CONNECT_RETRIED";
        case SZMQ_EVENT_LISTENING:
            return "LISTENING";
        case SZMQ_EVENT_BIND_FAILED:
            return "BIND_FAILED";
        case SZMQ_EVENT_ACCEPTED:
            return "ACCEPTED";
        case SZMQ_EVENT_ACCEPT_FAILED:
            return "ACCEPT_FAILED";
        case SZMQ_EVENT_CLOSED:
            return "CLOSED";
        case SZMQ_EVENT_CLOSE_FAILED:
            return "CLOSE_FAILED";
        case SZMQ_EVENT_DISCONNECTED:
            return "DISCONNECTED";
        case SZMQ_EVENT_MONITOR_STOPPED:
            return "MONITOR_STOPPED";
        case SZMQ_EVENT_HANDSHAKE_FAILED_NO_DETAIL:
            return "HANDSHAKE_FAILED_NO_DETAIL";
        case SZMQ_EVENT_HANDSHAKE_SUCCEEDED:
            return "HANDSHAKE_SUCCEEDED";
        case SZMQ_EVENT_HANDSHAKE_FAILED_PROTOCOL:
            return "HANDSHAKE_FAILED_PROTOCOL";
        case SZMQ_EVENT_HANDSHAKE_FAILED_AUTH:
            return "HANDSHAKE_FAILED_AUTH";
        default:
            return "UNKNOWN";
    }
}

void print_events (void *socket, int timeout, int limit)
{
    // print events received
    int value;
    char *event_address;
    int event =
      get_monitor_event_with_timeout (socket, &value, &event_address, timeout);
    int i = 0;
    ;
    while ((event != -1) && (++i < limit)) {
        const char *eventName = get_zmqEventName (event);
        printf ("Got event: %s\n", eventName);
        event = get_monitor_event_with_timeout (socket, &value, &event_address,
                                                timeout);
    }
}
