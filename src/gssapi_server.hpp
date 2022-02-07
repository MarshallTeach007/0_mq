

#ifndef __SZMQ_GSSAPI_SERVER_HPP_INCLUDED__
#define __SZMQ_GSSAPI_SERVER_HPP_INCLUDED__

#ifdef HAVE_LIBGSSAPI_KRB5

#include "gssapi_mechanism_base.hpp"
#include "zap_client.hpp"

namespace zmq
{
class msg_t;
class session_base_t;

class gssapi_server_t SZMQ_FINAL : public gssapi_mechanism_base_t,
                                  public zap_client_t
{
  public:
    gssapi_server_t (session_base_t *session_,
                     const std::string &peer_address,
                     const options_t &options_);
    ~gssapi_server_t () SZMQ_FINAL;

    // mechanism implementation
    int next_handshake_command (msg_t *msg_) SZMQ_FINAL;
    int process_handshake_command (msg_t *msg_) SZMQ_FINAL;
    int encode (msg_t *msg_) SZMQ_FINAL;
    int decode (msg_t *msg_) SZMQ_FINAL;
    int zap_msg_available () SZMQ_FINAL;
    status_t status () const SZMQ_FINAL;

  private:
    enum state_t
    {
        send_next_token,
        recv_next_token,
        expect_zap_reply,
        send_ready,
        recv_ready,
        connected
    };

    session_base_t *const session;

    const std::string peer_address;

    //  Current FSM state
    state_t state;

    //  True iff server considers the client authenticated
    bool security_context_established;

    //  The underlying mechanism type (ignored)
    gss_OID doid;

    void accept_context ();
    int produce_next_token (msg_t *msg_);
    int process_next_token (msg_t *msg_);
    void send_zap_request ();
};
}

#endif

#endif
