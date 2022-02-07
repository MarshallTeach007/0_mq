

#ifndef __SZMQ_V1_ENCODER_HPP_INCLUDED__
#define __SZMQ_V1_ENCODER_HPP_INCLUDED__

#include "encoder.hpp"

namespace zmq {
//  Encoder for ZMTP/1.0 protocol. Converts messages into data batches.

    class v1_encoder_t SZMQ_FINAL : public encoder_base_t<v1_encoder_t> {
    public:
        v1_encoder_t(size_t bufsize_);

        ~v1_encoder_t();

    private:
        void size_ready();

        void message_ready();

        unsigned char _tmpbuf[11];

    SZMQ_NON_COPYABLE_NOR_MOVABLE (v1_encoder_t)
    };
}

#endif
