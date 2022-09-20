#pragma once
#ifndef __HTTPSERVER_C
#define __HTTPSERVER_C
static void handle_sigterm(int signum);
////////////////////////////////////////////
#include "httpserver-utils/httpserver-utils.h"
#include "httpserver.h/httpserver.h"
#include "httpserver/httpserver.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "libforks/libforks.h"
#include "log/log.h"
#include "ms/ms.h"
#include "querystring.c/querystring.h"
#include "timestamp/timestamp.h"
#include "url.h/url.h"
#include "websocket-server/websocket-server.h"
#include <pthread.h>
////////////////////////////////////////////
static bool                HTTPSERVER_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static libforks_ServerConn conn;
static void __attribute__((constructor)) __constructor__httpserver(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_httpserver") != NULL) {
    log_debug("Enabling httpserver Debug Mode");
    HTTPSERVER_DEBUG_MODE = true;
  }

  assert(libforks_start(&conn) == libforks_OK);
  pid_t fork_server_pid = libforks_get_server_pid(conn);

  assert(fork_server_pid > 0);
  log_info(AC_YELLOW "Webserver> Fork server with pid %d created"AC_RESETALL, fork_server_pid);
}

void req_wrapper(struct http_request_s *request){
  struct ud_t *ud = (struct ud_t *)http_request_server_userdata(request);

  ud->requests_qty++;
  struct rq_t *rq = &(struct rq_t){
    .started = timestamp(),
    .pid     = getpid(),
    .ok      = true,
    .parsed  = calloc(1, sizeof(url_data_t)),
    .pd      = calloc(1, sizeof(struct parsed_data_t)),
  };

  http_request_set_userdata(request, (void *)rq);
  handle_request(request);
  rq      = (struct rq_t *)http_request_userdata(request);
  rq->dur = timestamp() - rq->started;
  log_info("Request #%lu handled in %s (" AC_YELLOW AC_REVERSED "%d" AC_RESETALL ")|ok:%d|size:%s|" AC_BLUE "[pd type:%s|id:%lu]|method:%s|pathname:%s|url:%s|path:%s|parse_url:%s|" AC_RESETALL,
           ud->requests_qty, milliseconds_to_string(rq->dur), rq->response_status, rq->ok, bytes_to_string(rq->size), rq->pd->type, rq->pd->id,
           rq->method, rq->parsed->pathname, rq->url, rq->path, rq->parse_url
           );
}

static int _httpserver_main(libforks_ServerConn conn, int socket_fd) {
  assert(libforks_free_conn(conn) == libforks_OK);
  pthread_t th;

  char      *msg;
  asprintf(&msg, "Starting Thread\n");
  write(socket_fd, msg, strlen(msg));
  asprintf(&msg, "Started Thread\n");
  write(socket_fd, msg, strlen(msg));
  pthread_create(&th, NULL, websocket_server, (void *)(DARWIN_LS_HTTPSERVER_PORT + 1));

  signal(SIGTERM, handle_sigterm);
  server = http_server_init(DARWIN_LS_HTTPSERVER_PORT, req_wrapper);

  struct ud_t ud = {
    .pid          = getpid(),
    .requests_qty = 0,
  };
  http_server_set_userdata(server, (void *)&ud);

  asprintf(&msg, "Server Iniitialized. Starting on port %d\n", DARWIN_LS_HTTPSERVER_PORT);
  write(socket_fd, msg, strlen(msg));
  http_server_listen_addr(server, DARWIN_LS_HTTPSERVER_HOST);
  asprintf(&msg, "Ended Thread\n");
  write(socket_fd, msg, strlen(msg));
}

static void handle_sigterm(int signum) {
  (void)signum;
  log_info("Webserver sigterm");
  if (server) {
    free(server);
  }
  exit(EXIT_SUCCESS);
}

int httpserver_main() {
  static int   socket_fd;
  static int   exit_fd;
  static pid_t pid;

  assert(libforks_fork(
           conn,
           &pid,
           &socket_fd,
           &exit_fd,
           _httpserver_main
           ) == libforks_OK);
  assert(pid > 0);
  printf(AC_YELLOW "Parent> Created child with pid %d\n"AC_RESETALL, pid);

  char *msg = calloc(1, 1024);

  while (true) {
    int read_res = read(socket_fd, msg, 1024);
    if (read_res == 0) {
      break;
    }
    log_debug(AC_RED "<PID %d>"AC_RESETALL " " AC_GREEN "%s" AC_RESETALL,
              pid,
              (char *)msg
              );
  }
}

////////////////////////////////////////////
////////////////////////////////////////////
#endif
