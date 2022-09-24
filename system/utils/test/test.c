#include <stdbool.h>
#define DEBUG_PID_ENV    false
//////////////////////////////////////////
#include "c_greatest/greatest/greatest.h"
#include "display/utils/utils.h"
#include "system/utils/test/test.h"
#include "process/utils/utils.h"

//////////////////////////////////////////
static int get_argmax() {
  int    argmax;
  int    mib[] = { CTL_KERN, KERN_ARGMAX };
  size_t size  = sizeof(argmax);

  if (sysctl(mib, 2, &argmax, &size, NULL, 0) == 0) {
    return(argmax);
  }
  return(0);
}

//////////////////////////////////////////
TEST t_display_id(void){
  CGDirectDisplayID display_id = get_display_id(0);

  fprintf(stdout,
          "display id:%d\n",
          display_id
          );
  PASS();
}

TEST t_display_count(void){
  int display_count = get_active_display_count();

  fprintf(stdout,
          "display count:%d\n",
          display_count
          );
  PASS();
}

TEST t_devices_count(void){
  size_t qty = get_devices_count();

  fprintf(stdout,
          "devices qty:%lu\n",
          qty
          );
  PASS();
}

TEST t_get_pid_info(void){
  pid_t             pid = getpid();
  struct kinfo_proc kp  = { 0 };
  int               res = get_kinfo_proc(pid, &kp);

  ASSERT_EQ(res, 0);
  fprintf(stdout,
          "pid:%d\n"
          "ppid:%d\n"
          "p_comm:%s\n"
          "e_xsize:%hu\n"
          "e_xccount:%hu\n"
          "e_xswrss:%hu\n"
          "e_wmesg:%s\n",
          pid,
          kp.kp_eproc.e_ppid,
          kp.kp_proc.p_comm,
          kp.kp_eproc.e_xrssize,
          kp.kp_eproc.e_xccount,
          kp.kp_eproc.e_xswrss,
          kp.kp_eproc.e_wmesg
          );

  PASS();
}

TEST t_get_cpu(void){
  double usage = get_cpu();
  double time  = get_cpu_time();

  fprintf(stdout,
          "cpu usage: %f\n"
          "cpu time:  %f\n",
          usage,
          time
          );

  PASS();
}

TEST t_get_gpu(void){
  get_gpu();
  PASS();
}

TEST t_get_disk(void){
  get_disk();
  PASS();
}

TEST t_get_model(void){
  get_model();
  PASS();
}

TEST t_get_mem(void){
  get_mem();
  PASS();
}

TEST t_display_size(void){
  CGDirectDisplayID              display_id = get_display_id(0);
  struct DarwinDisplayResolution *res       = get_display_resolution(display_id);

  fprintf(stdout,
          "x:%zu|y:%zu|pixels:%d|width:%fmm|height:%fmm|\n",
          res->x,
          res->y,
          res->pixels,
          res->size.width,
          res->size.height
          );
  PASS();
}

SUITE(s_display_count){
  RUN_TEST(t_display_count);
  PASS();
}

SUITE(s_display_id){
  RUN_TEST(t_display_id);
  PASS();
}

SUITE(s_get_pid_info){
  RUN_TEST(t_get_pid_info);
  PASS();
}

SUITE(s_get_cpu){
  RUN_TEST(t_get_cpu);
  PASS();
}

SUITE(s_get_gpu){
  RUN_TEST(t_get_gpu);
  PASS();
}
SUITE(s_get_disk){
  RUN_TEST(t_get_disk);
  PASS();
}

SUITE(s_get_model){
  RUN_TEST(t_get_model);
  PASS();
}

SUITE(s_get_mem){
  RUN_TEST(t_get_mem);
  PASS();
}

SUITE(s_devices_count){
  RUN_TEST(t_devices_count);
  PASS();
}

SUITE(s_display_size){
  RUN_TEST(t_display_size);
  PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_display_id);
  RUN_SUITE(s_display_size);
  RUN_SUITE(s_display_count);
  RUN_SUITE(s_devices_count);
  RUN_SUITE(s_get_mem);
  RUN_SUITE(s_get_model);
  RUN_SUITE(s_get_disk);
  RUN_SUITE(s_get_gpu);
  RUN_SUITE(s_get_cpu);
  RUN_SUITE(s_get_pid_info);
  GREATEST_MAIN_END();
  return(0);
}
