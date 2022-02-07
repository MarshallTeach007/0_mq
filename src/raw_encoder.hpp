

#ifndef __SZMQ_RAW_ENCODER_HPP_INCLUDED__
#define __SZMQ_RAW_ENCODER_HPP_INCLUDED__

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "encoder.hpp"

namespace zmq {
//  Encoder for 0MQ framing protocol. Converts messages into data batches.

    class raw_encoder_t SZMQ_FINAL : public encoder_base_t<raw_encoder_t> {
    public:
        raw_encoder_t(size_t bufsize_);

        ~raw_encoder_t();

    private:
        void raw_message_ready();

    SZMQ_NON_COPYABLE_NOR_MOVABLE (raw_encoder_t)
    };
}

#endif
