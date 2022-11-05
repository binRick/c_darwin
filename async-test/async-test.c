
#include "ansi-codes/ansi-codes.h"
#include "async-test/async-test.h"
#include "async/async.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

TEST t_async_test2(){
  struct Vector *v[3];
  struct Vector *results[30];
  for(int i=0;i<3;i++){
    v[i] = vector_new();
    vector_push(v[i],(void*)(size_t)(i+100));
    vector_push(v[i],(void*)(size_t)(500));
  }
  wi_async_iterator wi = ^void*(size_t index, size_t qty, void*item, void *res){
    log_info("#%lu/%lu>",index+1,qty);
    size_t s;
    if(res)
      s = (size_t)res + (size_t)item;
    else
      s = (size_t)item;
    return((void*)s);
  };
  async_iterator I = ^void*(size_t index, size_t qty, void*item){
    log_info("#%lu/%lu>",index+1,qty);
    size_t s;
    s = ((size_t)item) * 2;
    return((void*)s);
  };

  AsyncSeries *s = AsyncSeries_load(v[0]);
  results[0] = AsyncSeries_iterate(s,I);
  Dbg(vector_size(results[0]),%lu);
  for(size_t i = 0; i <vector_size(results[0]);i++)
    Dbg((size_t)vector_get(results[0],i),%lu);

  AsyncMap *m = AsyncMap_load(v[1]);
  results[1] = AsyncMap_iterate(s,I,1);
  Dbg(vector_size(results[1]),%lu);
  for(size_t i = 0; i <vector_size(results[1]);i++)
    Dbg((size_t)vector_get(results[1],i),%lu);

  AsyncWaterfall *w = AsyncWaterfall_load(v[2]);
  results[2] = AsyncWaterfall_iterate(s,wi);
  Dbg(vector_size(results[2]),%lu);
  for(size_t i = 0; i <vector_size(results[2]);i++)
    Dbg((size_t)vector_get(results[2],i),%lu);
  PASS();
}

TEST t_async_test1(){
  struct Vector *v[3];
  struct Vector *results[30];
  for(int i=0;i<3;i++){
    v[i] = vector_new();
    char *s;
    asprintf(&s,"%d",i);
    vector_push(v[i],(void*)s);
    vector_push(v[i],(void*)"12399");
  }
  wi_async_iterator wi = ^void*(size_t index, size_t qty, void*item, void *res){
    log_info("#%lu/%lu>",index+1,qty);
    char *s;
    if(res)
      asprintf(&s,"%s,%s",(char*)res,(char*)item);
    else
      s = (char*)item;
    return((char*)s);
  };
  async_iterator I = ^void*(size_t index, size_t qty, void*item){
    log_info("#%lu/%lu>",index+1,qty);
    char *s;
    asprintf(&s,"%s",(char*)item);
    return((void*)s);
  };

  AsyncSeries *s = AsyncSeries_load(v[0]);
  results[0] = AsyncSeries_iterate(s,I);
  Dbg(vector_size(results[0]),%lu);
  for(size_t i = 0; i <vector_size(results[0]);i++)
    Dbg((char*)vector_get(results[0],i),%s);

  results[10] = AsyncSeries_iterate(s,I);
  Dbg(vector_size(results[10]),%lu);
  for(size_t i = 0; i <vector_size(results[10]);i++)
    Dbg((char*)vector_get(results[10],i),%s);

  AsyncMap *m = AsyncMap_load(v[1]);
  results[1] = AsyncMap_iterate(s,I,1);
  Dbg(vector_size(results[1]),%lu);
  for(size_t i = 0; i <vector_size(results[1]);i++)
    Dbg((char*)vector_get(results[1],i),%s);

  AsyncWaterfall *w = AsyncWaterfall_load(v[2]);
  results[2] = AsyncWaterfall_iterate(s,wi);
  Dbg(vector_size(results[2]),%lu);
  for(size_t i = 0; i <vector_size(results[2]);i++)
    Dbg((char*)vector_get(results[2],i),%s);

  PASS();
}

SUITE(s_async_test) {
  RUN_TEST(t_async_test1);
  RUN_TEST(t_async_test2);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_async_test);
  GREATEST_MAIN_END();
}
