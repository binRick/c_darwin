#pragma once
#ifndef WEBSOCKET_SERVER_C
#define WEBSOCKET_SERVER_C
static void handle_sigterm(int signum);
////////////////////////////////////////////
#include "websocket-server/websocket-server.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "chan/src/chan.h"
#include "flingfd/src/flingfd.h"
#include "libforks/libforks.h"
#include "log/log.h"
#include "ms/ms.h"
#include "socket-protocol/socket-protocol.h"
#include "timestamp/timestamp.h"
#include "uuid4/src/uuid4.h"
static struct websocketserver_chan_t websocket_chans[] = {
  [WEBSOCKET_SERVER_CONNECTION]  = { .chan = NULL, .buffer_qty = 10,  },
  [WEBSOCKET_SERVER_NEW_SESSION] = { .chan = NULL, .buffer_qty = 100, },
  [WEBSOCKET_SERVER_MESSAGE]     = { .chan = NULL, .buffer_qty = 10,  },
};
static size_t                        connections_qty = 0;
static bool                          WEBSOCKET_SERVER_DEBUG_MODE = false;
static libforks_ServerConn           conn_websocket_server, conn_websocket_mon;

///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__websocket_server(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_websocket_server") != NULL) {
    log_debug("Enabling websocket-server Debug Mode");
    WEBSOCKET_SERVER_DEBUG_MODE = true;
  }

  assert(libforks_start(&conn_websocket_mon) == libforks_OK);
  pid_t fork_mon_pid = libforks_get_server_pid(conn_websocket_mon);
  assert(fork_mon_pid > 0);
  if (WEBSOCKET_SERVER_DEBUG_MODE) {
    log_info(AC_YELLOW "Mon Server> Fork server with pid %d created"AC_RESETALL, fork_mon_pid);
  }

  assert(libforks_start(&conn_websocket_server) == libforks_OK);
  pid_t fork_server_pid = libforks_get_server_pid(conn_websocket_server);

  assert(fork_server_pid > 0);
  if (WEBSOCKET_SERVER_DEBUG_MODE) {
    log_info(AC_YELLOW "Websocket Server> Fork server with pid %d created"AC_RESETALL, fork_server_pid);
  }
  signal(SIGTERM, handle_sigterm);
}

struct session_t {
  int                       fd;
  char                      uuid[UUID4_LEN];
  const unsigned long       started;
  pid_t                     pid;
  size_t                    bytes_read, bytes_written, sent_msgs, recvd_msgs;
  struct websocket_config_t *c;
  int                       mypipe;
};

void write_to_pipe(int file, char *msg) {
  FILE *stream;

  stream = fdopen(file, "w");
  fprintf(stream, "%s\n", msg);
  fclose(stream);
}

int _read_from_pipe(void *f){
  int                 file = (int)(size_t)f;
  FILE                *stream;
  int                 c;
  char                *m;
  struct StringBuffer *sb;

  stream = fdopen(file, "r");
  sb     = stringbuffer_new();
  while ((c = fgetc(stream)) != EOF) {
    stringbuffer_append(sb, c);
  }
  if (strlen(stringbuffer_to_string(sb)) > 0) {
    m = stringbuffer_to_string(sb);
    log_info("READ> %s", m);
  }
  fclose(stream);
  stringbuffer_release(sb);
  return(0);
}

char *read_from_pipe(int file) {
  pthread_t th;

  pthread_create(&th, NULL, _read_from_pipe, (void *)file);
  pthread_join(&th, NULL);
  return(NULL);
}

////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "wslay-test/wslay-test.h"
#include "wslay/wslay.h"
#include <pthread.h>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <nettle/base64.h>
#include <nettle/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wslay/wslay.h>

static int create_listen_socket(const char *service) {
  struct addrinfo hints, *res, *rp;
  int             sfd = -1;
  int             r;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE | AI_ADDRCONFIG;
  r                 = getaddrinfo(0, service, &hints, &res);
  if (r != 0) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(r));
    return(-1);
  }
  for (rp = res; rp; rp = rp->ai_next) {
    int val = 1;
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1) {
      continue;
    }
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &val,
                   (socklen_t)sizeof(val)) == -1) {
      continue;
    }
    if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
      break;
    }
    close(sfd);
  }
  freeaddrinfo(res);
  if (listen(sfd, 16) == -1) {
    perror("listen");
    close(sfd);
    return(-1);
  }
  return(sfd);
}

/*
 * Makes file descriptor *fd* non-blocking mode.
 * This function returns 0, or returns -1.
 */
static int make_non_block(int fd) {
  int flags, r;

  while ((flags = fcntl(fd, F_GETFL, 0)) == -1 && errno == EINTR) {
  }
  if (flags == -1) {
    perror("fcntl");
    return(-1);
  }
  while ((r = fcntl(fd, F_SETFL, flags | O_NONBLOCK)) == -1 && errno == EINTR) {
  }
  if (r == -1) {
    perror("fcntl");
    return(-1);
  }
  return(0);
}

/*
 * Calculates SHA-1 hash of *src*. The size of *src* is *src_length* bytes.
 * *dst* must be at least SHA1_DIGEST_SIZE.
 */
static void sha1(uint8_t *dst, const uint8_t *src, size_t src_length) {
  struct sha1_ctx ctx;

  sha1_init(&ctx);
  sha1_update(&ctx, src_length, src);
  sha1_digest(&ctx, SHA1_DIGEST_SIZE, dst);
}

/*
 * Base64-encode *src* and stores it in *dst*.
 * The size of *src* is *src_length*.
 * *dst* must be at least BASE64_ENCODE_RAW_LENGTH(src_length).
 */
static void base64(uint8_t *dst, const uint8_t *src, size_t src_length) {
  struct base64_encode_ctx ctx;

  base64_encode_init(&ctx);
  base64_encode_raw((char *)dst, src_length, src);
}

#define WS_GUID    "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

/*
 * Create Server's accept key in *dst*.
 * *client_key* is the value of |Sec-WebSocket-Key| header field in
 * client's handshake and it must be length of 24.
 * *dst* must be at least BASE64_ENCODE_RAW_LENGTH(20)+1.
 */
static void create_accept_key(char *dst, const char *client_key) {
  uint8_t sha1buf[20], key_src[60];

  memcpy(key_src, client_key, 24);
  memcpy(key_src + 24, WS_GUID, 36);
  sha1(sha1buf, key_src, sizeof(key_src));
  base64((uint8_t *)dst, sha1buf, 20);
  dst[BASE64_ENCODE_RAW_LENGTH(20)] = '\0';
}

/* We parse HTTP header lines of the format
 *   \r\nfield_name: value1, value2, ... \r\n
 *
 * If the caller is looking for a specific value, we return a pointer to the
 * start of that value, else we simply return the start of values list.
 */
static const char *http_header_find_field_value(const char *header,
                                                const char *field_name,
                                                const char *value) {
  const char *header_end, *field_start, *field_end, *next_crlf, *value_start;
  int        field_name_len;

  /* Pointer to the last character in the header */
  header_end = header + strlen(header) - 1;

  field_name_len = (int)strlen(field_name);

  field_start = header;

  do {
    field_start = strstr(field_start + 1, field_name);

    field_end = field_start + field_name_len - 1;

    if (field_start != NULL && field_start - header >= 2
        && field_start[-2] == '\r' && field_start[-1] == '\n'
        && header_end - field_end >= 1 && field_end[1] == ':') {
      break;    /* Found the field */
    } else {
      continue; /* This is not the one; keep looking. */
    }
  } while (field_start != NULL);

  if (field_start == NULL) {
    return(NULL);
  }

  /* Find the field terminator */
  next_crlf = strstr(field_start, "\r\n");

  /* A field is expected to end with \r\n */
  if (next_crlf == NULL) {
    return(NULL); /* Malformed HTTP header! */
  }
  /* If not looking for a value, then return a pointer to the start of values string */
  if (value == NULL) {
    return(field_end + 2);
  }

  value_start = strstr(field_start, value);

  /* Value not found */
  if (value_start == NULL) {
    return(NULL);
  }

  /* Found the value we're looking for */
  if (value_start > next_crlf) {
    return(NULL); /* ... but after the CRLF terminator of the field. */
  }
  /* The value we found should be properly delineated from the other tokens */
  if (isalnum(value_start[-1]) || isalnum(value_start[strlen(value)])) {
    return(NULL);
  }

  return(value_start);
} /* http_header_find_field_value */

/*
 * Performs HTTP handshake. *fd* is the file descriptor of the
 * connection to the client. This function returns 0 if it succeeds,
 * or returns -1.
 */
static int http_handshake(int fd) {
  /*
   * Note: The implementation of HTTP handshake in this function is
   * written for just a example of how to use of wslay library and is
   * not meant to be used in production code.  In practice, you need
   * to do more strict verification of the client's handshake.
   */
  char       header[16384], accept_key[29], res_header[256];
  const char *keyhdstart, *keyhdend;
  size_t     header_length = 0, res_header_sent = 0, res_header_length;
  ssize_t    r;

  while (1) {
    while ((r = read(fd, header + header_length,
                     sizeof(header) - header_length)) == -1
           && errno == EINTR) {
    }
    if (r == -1) {
      perror("read");
      return(-1);
    } else if (r == 0) {
      fprintf(stderr, "HTTP Handshake: Got EOF");
      return(-1);
    } else {
      header_length += (size_t)r;
      if (header_length >= 4
          && memcmp(header + header_length - 4, "\r\n\r\n", 4) == 0) {
        break;
      } else if (header_length == sizeof(header)) {
        fprintf(stderr, "HTTP Handshake: Too large HTTP headers");
        return(-1);
      }
    }
  }

  if (http_header_find_field_value(header, "Upgrade", "websocket") == NULL
      || http_header_find_field_value(header, "Connection", "Upgrade") == NULL
      || (keyhdstart = http_header_find_field_value(header, "Sec-WebSocket-Key",
                                                    NULL)) == NULL) {
    fprintf(stderr, "HTTP Handshake: Missing required header fields");
    return(-1);
  }
  for ( ; *keyhdstart == ' '; ++keyhdstart) {
  }
  keyhdend = keyhdstart;
  for ( ; *keyhdend != '\r' && *keyhdend != ' '; ++keyhdend) {
  }
  if (keyhdend - keyhdstart != 24) {
    printf("%s\n", keyhdstart);
    fprintf(stderr, "HTTP Handshake: Invalid value in Sec-WebSocket-Key");
    return(-1);
  }
  create_accept_key(accept_key, keyhdstart);
  snprintf(res_header, sizeof(res_header),
           "HTTP/1.1 101 Switching Protocols\r\n"
           "Upgrade: websocket\r\n"
           "Connection: Upgrade\r\n"
           "Sec-WebSocket-Accept: %s\r\n"
           "\r\n",
           accept_key);
  res_header_length = strlen(res_header);
  while (res_header_sent < res_header_length) {
    while ((r = write(fd, res_header + res_header_sent,
                      res_header_length - res_header_sent)) == -1
           && errno == EINTR) {
    }
    if (r == -1) {
      perror("write");
      return(-1);
    } else {
      res_header_sent += (size_t)r;
    }
  }
  return(0);
} /* http_handshake */

/*
 * This struct is passed as *user_data* in callback function.  The
 * *fd* member is the file descriptor of the connection to the client.
 */
static ssize_t send_callback(wslay_event_context_ptr ctx, const uint8_t *data,
                             size_t len, int flags, void *user_data) {
  struct session_t *session = (struct session_t *)user_data;

  ssize_t          r;
  int              sflags = 0;

#ifdef MSG_MORE
  if (flags & WSLAY_MSG_MORE) {
    sflags |= MSG_MORE;
  }
#endif // MSG_MORE
  while ((r = send(session->fd, data, len, sflags)) == -1 && errno == EINTR) {
  }
  if (r == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      wslay_event_set_error(ctx, WSLAY_ERR_WOULDBLOCK);
    } else {
      wslay_event_set_error(ctx, WSLAY_ERR_CALLBACK_FAILURE);
    }
  }
  return(r);
}

static ssize_t recv_callback(wslay_event_context_ptr ctx, uint8_t *buf,
                             size_t len, int flags, void *user_data) {
  struct session_t *session = (struct session_t *)user_data;
  ssize_t          r;

  (void)flags;
  while ((r = recv(session->fd, buf, len, 0)) == -1 && errno == EINTR) {
  }
  if (r == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      wslay_event_set_error(ctx, WSLAY_ERR_WOULDBLOCK);
    } else {
      wslay_event_set_error(ctx, WSLAY_ERR_CALLBACK_FAILURE);
    }
  } else if (r == 0) {
    /* Unexpected EOF is also treated as an error */
    wslay_event_set_error(ctx, WSLAY_ERR_CALLBACK_FAILURE);
    r = -1;
  }
  return(r);
}

static void on_msg_recv_callback(wslay_event_context_ptr                  ctx,
                                 const struct wslay_event_on_msg_recv_arg *arg,
                                 void                                     *user_data) {
  (void)user_data;
  struct session_t *session   = (struct session_t *)user_data;
  char             *recvd_msg = stringfn_substring(arg->msg, 0, arg->msg_length);
  log_debug("Client Sent %s msg: %s", bytes_to_string(arg->msg_length), recvd_msg);
  session->sent_msgs++;
  session->bytes_written += arg->msg_length;

  /* Echo back non-control message */
  if (!wslay_is_ctrl_frame(arg->opcode)) {
    log_debug("Write Enabled? %s", wslay_event_get_write_enabled(ctx) == 1 ? "Yes":"No");
    char                   *buf;
    asprintf(&buf, "%lu", (size_t)timestamp());
    struct wslay_event_msg msg = { WSLAY_TEXT_FRAME, buf, strlen(buf) };
    int                    ok  = wslay_event_queue_msg(ctx, &msg);
    char                   *m;
    asprintf(&m, "recv callback");
    log_info("%s", m);
    if (ok == 0) {
      log_debug("Queued %s msg to %s", bytes_to_string(strlen(buf)), session->uuid);
    }else{
      log_error("Failed to queue msg: %d", ok);
    }
    log_debug("%lu queued msgs", wslay_event_get_queued_msg_length(ctx));
    asprintf(&m, "<%d>  recv|%s|%lu|%d|%d|", getpid(), arg->msg, arg->msg_length, arg->opcode, arg->status_code);
    //write_to_pipe(session->mypipe,m);
    struct wslay_event_msg msgarg = { arg->opcode, arg->msg, arg->msg_length };
    wslay_event_queue_msg(ctx, &msgarg);
    wslay_event_queue_msg(ctx, &msgarg);
  }
}

/*
 * Communicate with the client. This function performs HTTP handshake
 * and WebSocket data transfer until close handshake is done or an
 * error occurs. *fd* is the file descriptor of the connection to the
 * client. This function returns 0 if it succeeds, or returns 0.
 */
static int communicate(int fd, void *C, int mypipe){
  struct websocket_config_t *c = (struct websocket_config_t *)C;
  char                      *msg;

  asprintf(&msg, "communicating");
  log_info("%s", msg);

  wslay_event_context_ptr      ctx;
  struct wslay_event_callbacks callbacks = {
    recv_callback, send_callback, NULL, NULL, NULL,
    NULL,          on_msg_recv_callback
  };
  struct session_t             session = {
    .fd            = fd,
    .started       = timestamp(),
    .pid           = getpid(),
    .sent_msgs     = 0,
    .recvd_msgs    = 0,
    .bytes_read    = 0,
    .bytes_written = 0,
    .c             = c,
    .mypipe        = mypipe,
  };

  uuid4_generate(session.uuid);
  asprintf(&msg, "sess %s", session.uuid);
  log_info("%s", msg);
  log_info("<%d> New Session |pid:%d|fd:%d|uuid:%s|started:%ld|", getpid(), session.pid, session.fd, session.uuid, session.started);

  int           val = 1;
  struct pollfd event;
  int           res = 0;

  if (http_handshake(fd) == -1) {
    return(-1);
  }
  if (make_non_block(fd) == -1) {
    return(-1);
  }
  if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val, (socklen_t)sizeof(val)) ==
      -1) {
    perror("setsockopt: TCP_NODELAY");
    return(-1);
  }
  memset(&event, 0, sizeof(struct pollfd));
  event.fd     = fd;
  event.events = POLLIN;
  wslay_event_context_server_init(&ctx, &callbacks, &session);
  connections_qty++;
  asprintf(&msg, "<%d> New Client Connection!  #%lu", getpid(), connections_qty);
  log_info("%s", msg);
  /*
   * Event loop: basically loop until both wslay_event_want_read(ctx)
   * and wslay_event_want_write(ctx) return 0.
   */
  while (wslay_event_want_read(ctx) || wslay_event_want_write(ctx)) {
    int r;
    while ((r = poll(&event, 1, -1)) == -1 && errno == EINTR) {
    }
    if (r == -1) {
      perror("poll");
      res = -1;
      break;
    }
    if (((event.revents & POLLIN) && wslay_event_recv(ctx) != 0)
        || ((event.revents & POLLOUT) && wslay_event_send(ctx) != 0)
        || (event.revents & (POLLERR | POLLHUP | POLLNVAL))) {
      /*
       * If either wslay_event_recv() or wslay_event_send() return
       * non-zero value, it means serious error which prevents wslay
       * library from processing further data, so WebSocket connection
       * must be closed.
       */
      res = -1;
      break;
    }
    event.events = 0;
    if (wslay_event_want_read(ctx)) {
      event.events |= POLLIN;
    }
    if (wslay_event_want_write(ctx)) {
      event.events |= POLLOUT;
    }
  }
  log_debug("<%d> Session Closing after %s |pid:%d|fd:%d|uuid:%s|started:%ld|"
            "%lu/%lu bytes read/written|%lu/%lu msgs sent/recvd|",
            getpid(), milliseconds_to_string(timestamp() - session.started),
            session.pid, session.fd, session.uuid, session.started,
            session.bytes_read, session.bytes_written, session.sent_msgs, session.recvd_msgs

            );
  return(res);
} /* communicate */

/*
 * Serves echo back service forever.  *sfd* is the file descriptor of
 * the server socket.  when the incoming connection from the client is
 * accepted, this function forks another process and the forked
 * process communicates with client. The parent process goes back to
 * the loop and can accept another client.
 */
static void __attribute__((noreturn)) serve(int sfd, void *C){
  struct websocket_config_t *c = (struct websocket_config_t *)C;

  while (1) {
    int fd;
    while ((fd = accept(sfd, NULL, NULL)) == -1 && errno == EINTR) {
    }
    int mypipe[2];
    if (pipe(mypipe)) {
      fprintf(stderr, "Pipe failed.\n");
      return(EXIT_FAILURE);
    }
    if (fd == -1) {
      perror("accept");
    } else {
      int r = fork();
      if (r == -1) {
        perror("fork");
        close(fd);
      } else if (r == 0) {
        close(mypipe[0]);
        r = communicate(fd, c, mypipe[1]);
        shutdown(fd, SHUT_WR);
        close(fd);
        if (r == 0) {
          exit(EXIT_SUCCESS);
        } else {
          exit(EXIT_FAILURE);
        }
      }else{
        log_info("child pid %d", r);
        close(mypipe[1]);
        //read_from_pipe(mypipe[0]);
      }
    }
  }
} /* serve */

int websocket_server(void *P){
  uuid4_init();
  struct websocket_config_t *c = (struct websocket_config_t *)P;
  char                      *msg;
  asprintf(&msg, "Websocket server binding to %s", c->socket);

  size_t retries = 0;
  bool   created = false;
  int    sfd;

  while (created == false) {
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &act, NULL);

    sfd = create_listen_socket(c->socket);
    msg = "socket bound";
    if (sfd == -1) {
      log_error("Failed to create server socket (%lu retries)", retries);
      retries++;
      sleep(1);
      if (retries > 100) {
        exit(EXIT_FAILURE);
      }
    }else{
      created = true;
    }
  }
  asprintf(&msg, "WebSocket echo server, listening on socket %s", c->socket);
  serve(sfd, (void *)c);
  return(EXIT_SUCCESS);
}

static void handle_sigterm(int signum) {
  (void)signum;
  log_info("Websocket sigterm");
  exit(EXIT_SUCCESS);
}

static int _websocket_server(libforks_ServerConn conn, void *P){
  libforks_free_conn(conn);
  struct libforks_param_t *p = (struct libforks_param_t *)P;
  char                    *msg;
  asprintf(&msg, "Starting Websocket server on port %d\n", p->websocket_server_port);
  log_error("%s", msg);

  write(p->socket_fd, msg, strlen(msg));
  return(websocket_server((void *)p));
}

#endif
