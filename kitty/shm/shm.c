#pragma once
#ifndef KITTY_SHM_C
#define KITTY_SHM_C
#define LOCAL_DEBUG_MODE    KITTY_SHM_DEBUG_MODE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
////////////////////////////////////////////
#include "kitty/shm/shm.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "ansi-utils/ansi-utils.h"
#include "b64.c/b64.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "vips/vips.h"

////////////////////////////////////////////
static bool KITTY_SHM_DEBUG_MODE = false;

#define STORAGE_ID                                     "/SHM_TEST"
#define STORAGE_SIZE                                   32
#define DATA                                           "Hello, World! From PID %d"
#define KITTY_SHM_SHM_FMT                              "\x1b_Gs=10,v=2,t=s,o=z;%s\x1b\\"
#define KITTY_SHM_SHM_IMAGE_PATH_FMT                   "\x1b_Gf=100,t=f;%s\x1b\\"
#define KITTY_SHM_SHM_ENCODED_IMAGE_PATH_DIMENSIONS    "\x1b_Ga=T,f=100,s=%ld,v=%ld,X=4,t=f;%s\x1b\\"

static const char *kitty_shm_fmt_msg(char *fmt, ...){
  char    *s;
  va_list vargs;

  va_start(vargs, fmt);
  va_end(vargs);
  asprintf(&s, fmt, vargs);
  return((const char *)s);
}

char *kitty_msg_get_display_image_by_path(char *image_path){
  char      *msg, *image_path_enc;
  size_t    width = 0, height = 0;
  VipsImage *image;

  if (!(image = vips_image_new_from_file(image_path, "access", VIPS_ACCESS_SEQUENTIAL, NULL))) {
    return(NULL);
  }
  width          = vips_image_get_width(image);
  height         = vips_image_get_height(image);
  image_path_enc = b64_encode(image_path, strlen(image_path));
  asprintf(&msg, KITTY_SHM_SHM_ENCODED_IMAGE_PATH_DIMENSIONS, width, height, image_path_enc);
  return(msg);
}

char *kitty_shm_get_shm_image_path_msg(char *image_path){
  size_t len   = fsio_file_size(image_path);
  char   *data = b64_encode(fsio_read_binary_file(image_path), len);
  char   *shm_path;

  asprintf(&shm_path, "/kitty-shm-%d-%lld", getpid(), timestamp());
  int shm_fd = shm_open(shm_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  if (shm_fd == -1) {
    perror("open");
    return(10);
  }
  int shm_res = ftruncate(shm_fd, len);

  if (shm_res == -1) {
    perror("ftruncate");
    return(20);
  }
  void *addr = mmap(NULL, len, PROT_WRITE, MAP_SHARED, shm_fd, 0);

  if (addr == MAP_FAILED) {
    perror("mmap");
    return(30);
  }
  memcpy(addr, data, len + 1);
  char *msg = kitty_shm_fmt_msg(shm_path, len);

  Dbg(shm_path, %s);
  Dbg(b64_encode(msg, strlen(msg)), %s);
  return(msg);
}

int kitty_shm_set(){
  int   res;
  int   fd;
  int   len;
  pid_t pid;
  void  *addr;
  char  data[STORAGE_SIZE];

  pid = getpid();
  sprintf(data, DATA, pid);

  // get shared memory file descriptor (NOT a file)
  fd = shm_open(STORAGE_ID, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    return(10);
  }

  // extend shared memory object as by default it's initialized with size 0
  res = ftruncate(fd, STORAGE_SIZE);
  if (res == -1) {
    perror("ftruncate");
    return(20);
  }

  // map shared memory to process address space
  addr = mmap(NULL, STORAGE_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    return(30);
  }

  // place data into memory
  len = strlen(data) + 1;
  memcpy(addr, data, len);

  // wait for someone to read it
  sleep(2);

  // mmap cleanup
  res = munmap(addr, STORAGE_SIZE);
  if (res == -1) {
    perror("munmap");
    return(40);
  }

  // shm_open cleanup
  fd = shm_unlink(STORAGE_ID);
  if (fd == -1) {
    perror("unlink");
    return(100);
  }

  return(0);
} /* kitty_shm_set */

int kitty_shm_get(){
  int   res;
  int   fd;
  char  data[STORAGE_SIZE];
  pid_t pid;
  void  *addr;

  pid = getpid();

  // get shared memory file descriptor (NOT a file)
  fd = shm_open(STORAGE_ID, O_RDONLY, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    return(10);
  }

  // map shared memory to process address space
  addr = mmap(NULL, STORAGE_SIZE, PROT_READ, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    return(30);
  }

  // place data into memory
  memcpy(data, addr, STORAGE_SIZE);

  printf("PID %d: Read from shared memory: \"%s\"\n", pid, data);

  return(0);
}

///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__kitty_shm(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_kitty_shm") != NULL) {
    log_debug("Enabling kitty_shm Debug Mode");
    KITTY_SHM_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
