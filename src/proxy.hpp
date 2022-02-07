

#ifndef __SZMQ_PROXY_HPP_INCLUDED__
#define __SZMQ_PROXY_HPP_INCLUDED__

namespace zmq {
    int proxy(class socket_base_t *frontend_,
              class socket_base_t *backend_,
              class socket_base_t *capture_,
              class socket_base_t *control_ =
              NULL); // backward compatibility without this argument
}

#endif
