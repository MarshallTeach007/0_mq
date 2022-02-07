

#ifndef __SZMQ_CURVE_CLIENT_HPP_INCLUDED__
#define __SZMQ_CURVE_CLIENT_HPP_INCLUDED__

#ifdef SZMQ_HAVE_CURVE

#include "curve_mechanism_base.hpp"
#include "options.hpp"
#include "curve_client_tools.hpp"

namespace zmq
{
class msg_t;
class session_base_t;

class curve_client_t SZMQ_FINAL : public curve_mechanism_base_t
{
  public:
    curve_client_t (session_base_t *session_,
                    const options_t &options_,
                    const bool downgrade_sub_);
    ~curve_client_t () SZMQ_FINAL;

    // mechanism implementation
    int next_handshake_command (msg_t *msg_) SZMQ_FINAL;
    int process_handshake_command (msg_t *msg_) SZMQ_FINAL;
    int encode (msg_t *msg_) SZMQ_FINAL;
    int decode (msg_t *msg_) SZMQ_FINAL;
    status_t status () const SZMQ_FINAL;

  private:
    enum state_t
    {
        send_hello,
        expect_welcome,
        send_initiate,
        expect_ready,
        error_received,
        connected
    };

    //  Current FSM state
    state_t _state;

    //  CURVE protocol tools
    curve_client_tools_t _tools;

    int produce_hello (msg_t *msg_);
    int process_welcome (const uint8_t *msg_data_, size_t msg_size_);
    int produce_initiate (msg_t *msg_);
    int process_ready (const uint8_t *msg_data_, size_t msg_size_);
    int process_error (const uint8_t *msg_data_, size_t msg_size_);
};
}

#endif

#endif
