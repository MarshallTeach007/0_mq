

#ifndef __SZMQ_DRAFT_H_INCLUDED__
#define __SZMQ_DRAFT_H_INCLUDED__

/******************************************************************************/
/*  These functions are DRAFT and disabled in stable releases, and subject to */
/*  change at ANY time until declared stable.                                 */
/******************************************************************************/

#ifndef SZMQ_BUILD_DRAFT_API

/*  DRAFT Socket types.                                                       */
#define SZMQ_SERVER 12
#define SZMQ_CLIENT 13
#define SZMQ_RADIO 14
#define SZMQ_DISH 15
#define SZMQ_GATHER 16
#define SZMQ_SCATTER 17
#define SZMQ_DGRAM 18
#define SZMQ_PEER 19
#define SZMQ_CHANNEL 20

/*  DRAFT Socket options.                                                     */
#define SZMQ_ZAP_ENFORCE_DOMAIN 93
#define SZMQ_LOOPBACK_FASTPATH 94
#define SZMQ_METADATA 95
#define SZMQ_MULTICAST_LOOP 96
#define SZMQ_ROUTER_NOTIFY 97
#define SZMQ_XPUB_MANUAL_LAST_VALUE 98
#define SZMQ_SOCKS_USERNAME 99
#define SZMQ_SOCKS_PASSWORD 100
#define SZMQ_IN_BATCH_SIZE 101
#define SZMQ_OUT_BATCH_SIZE 102
#define SZMQ_WSS_KEY_PEM 103
#define SZMQ_WSS_CERT_PEM 104
#define SZMQ_WSS_TRUST_PEM 105
#define SZMQ_WSS_HOSTNAME 106
#define SZMQ_WSS_TRUST_SYSTEM 107
#define SZMQ_ONLY_FIRST_SUBSCRIBE 108
#define SZMQ_RECONNECT_STOP 109
#define SZMQ_HELLO_MSG 110
#define SZMQ_DISCONNECT_MSG 111
#define SZMQ_PRIORITY 112
#define SZMQ_BUSY_POLL 113
#define SZMQ_HICCUP_MSG 114
#define SZMQ_XSUB_VERBOSE_UNSUBSCRIBE 115

/*  DRAFT SZMQ_RECONNECT_STOP options                                          */
#define SZMQ_RECONNECT_STOP_CONN_REFUSED 0x1
#define SZMQ_RECONNECT_STOP_HANDSHAKE_FAILED 0x2
#define SZMQ_RECONNECT_STOP_AFTER_DISCONNECT 0x4

/*  DRAFT Context options                                                     */
#define SZMQ_ZERO_COPY_RECV 10

/*  DRAFT Context methods.                                                    */
int zmq_ctx_set_ext(void *context_,
                    int option_,
                    const void *optval_,
                    size_t optvallen_);

int zmq_ctx_get_ext(void *context_,
                    int option_,
                    void *optval_,
                    size_t *optvallen_);

/*  DRAFT Socket methods.                                                     */
int zmq_join(void *s_, const char *group_);

int zmq_leave(void *s_, const char *group_);

/*  DRAFT Msg methods.                                                        */
int zmq_msg_set_routing_id(zmq_msg_t *msg_, uint32_t routing_id_);

uint32_t zmq_msg_routing_id(zmq_msg_t *msg_);

int zmq_msg_set_group(zmq_msg_t *msg_, const char *group_);

const char *zmq_msg_group(zmq_msg_t *msg_);

int zmq_msg_init_buffer(zmq_msg_t *msg_, const void *buf_, size_t size_);

/*  DRAFT Msg property names.                                                 */
#define SZMQ_MSG_PROPERTY_ROUTING_ID "Routing-Id"
#define SZMQ_MSG_PROPERTY_SOCKET_TYPE "Socket-Type"
#define SZMQ_MSG_PROPERTY_USER_ID "User-Id"
#define SZMQ_MSG_PROPERTY_PEER_ADDRESS "Peer-Address"

/*  Router notify options                                                     */
#define SZMQ_NOTIFY_CONNECT 1
#define SZMQ_NOTIFY_DISCONNECT 2

/******************************************************************************/
/*  Poller polling on sockets,fd and thread-safe sockets                      */
/******************************************************************************/

#if defined _WIN32
typedef SOCKET zmq_fd_t;
#else
typedef int zmq_fd_t;
#endif

typedef struct zmq_poller_event_t {
    void *socket;
    zmq_fd_t fd;
    void *user_data;
    short events;
} zmq_poller_event_t;

void *zmq_poller_new(void);

int zmq_poller_destroy(void **poller_p_);

int zmq_poller_size(void *poller_);

int zmq_poller_add(void *poller_,
                   void *socket_,
                   void *user_data_,
                   short events_);

int zmq_poller_modify(void *poller_, void *socket_, short events_);

int zmq_poller_remove(void *poller_, void *socket_);

int zmq_poller_wait(void *poller_, zmq_poller_event_t *event_, long timeout_);

int zmq_poller_wait_all(void *poller_,
                        zmq_poller_event_t *events_,
                        int n_events_,
                        long timeout_);

zmq_fd_t zmq_poller_fd(void *poller_);

int zmq_poller_add_fd(void *poller_,
                      zmq_fd_t fd_,
                      void *user_data_,
                      short events_);

int zmq_poller_modify_fd(void *poller_, zmq_fd_t fd_, short events_);

int zmq_poller_remove_fd(void *poller_, zmq_fd_t fd_);

int zmq_socket_get_peer_state(void *socket_,
                              const void *routing_id_,
                              size_t routing_id_size_);

/*  DRAFT Socket monitoring events                                            */
#define SZMQ_EVENT_PIPES_STATS 0x10000

#define SZMQ_CURRENT_EVENT_VERSION 1
#define SZMQ_CURRENT_EVENT_VERSION_DRAFT 2

#define SZMQ_EVENT_ALL_V1 SZMQ_EVENT_ALL
#define SZMQ_EVENT_ALL_V2 SZMQ_EVENT_ALL_V1 | SZMQ_EVENT_PIPES_STATS

int zmq_socket_monitor_versioned(
        void *s_, const char *addr_, uint64_t events_, int event_version_, int type_);

int zmq_socket_monitor_pipes_stats(void *s_);

#if !defined _WIN32

int zmq_ppoll(zmq_pollitem_t *items_,
              int nitems_,
              long timeout_,
              const sigset_t *sigmask_);

#else
// Windows has no sigset_t
int zmq_ppoll (zmq_pollitem_t *items_,
               int nitems_,
               long timeout_,
               const void *sigmask_);
#endif

#endif // SZMQ_BUILD_DRAFT_API

#endif //ifndef __SZMQ_DRAFT_H_INCLUDED__
