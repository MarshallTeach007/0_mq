

#ifndef __SZMQ_V2_PROTOCOL_HPP_INCLUDED__
#define __SZMQ_V2_PROTOCOL_HPP_INCLUDED__

namespace zmq {
//  Definition of constants for ZMTP/2.0 transport protocol.
    class v2_protocol_t {
    public:
        //  Message flags.
        enum {
            more_flag = 1,
            large_flag = 2,
            command_flag = 4
        };
    };
}

#endif
