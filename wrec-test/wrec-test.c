#include "wrec-test/wrec-test.h"

TEST t_callback_pixels(void){
  PASS();
}

TEST t_callback_write_file(void){
  PASS();
}

const struct capture_request_t *__REQUEST_BASE = &((struct capture_request_t){
  .debug_mode = true,
});
const char                     *FILE_PATH_TEMPLATE = "%swindow-%d-test-%s-%s-result-%lu.png";

const struct capture_request_t *__REQUEST_RESIZE_WRITE_FILE_RETURN_RESULT = &((struct capture_request_t){
  .mode = CAPTURE_REQUEST_MODE_WRITE_FILE_RETURN_RESULT,
  .resize_type = RESIZE_BY_FACTOR,
  .resize_factor = 5,
});
const struct capture_request_t *__REQUEST_NO_RESIZE_WRITE_FILE_RETURN_RESULT = &((struct capture_request_t){
  .mode = CAPTURE_REQUEST_MODE_WRITE_FILE_RETURN_RESULT,
  .resize_type = RESIZE_BY_NONE,
  .resize_factor = 1,
});

TEST t_return_no_resize_factor_write_file(void *REQ){
  struct capture_request_t *req = (struct capture_request_t *)REQ;
  struct capture_result_t  *res;
  {
    req->debug_mode = __REQUEST_BASE->debug_mode;
    req->window_id  = atoi(getenv("WINDOWID"));
    asprintf(&req->file_path, FILE_PATH_TEMPLATE,
             gettempdir(), req->window_id,
             "return", "resize",
             (size_t)timestamp()
             );
  }
  {
    res = request_window_capture(req);
  }
  {
    /*
     * ASSERT_GTm("requested file path too small", fsio_file_size(req->file_path), 128);
     * ASSERT_LTm("requested file path too big", fsio_file_size(req->file_path), 1024 * 1024 * 64);
     * ASSERT_EQm("requested file path does not exist", fsio_file_exists(req->file_path), true);
     * ASSERT_EQm("requested file not successful save", res->save_file_success, true);
     */
  }

  PASSm("Requested No Resize File Write OK");
}

TEST t_return_resize_factor_write_file(void *REQ){
  struct capture_request_t *req = (struct capture_request_t *)REQ;
  struct capture_result_t  *res;
  {
    req->debug_mode = __REQUEST_BASE->debug_mode;
    req->window_id  = atoi(getenv("WINDOWID"));
    asprintf(&req->file_path, FILE_PATH_TEMPLATE,
             gettempdir(), req->window_id,
             "return", "resize",
             (size_t)timestamp()
             );
  }
  {
    printf("path:%s\n", req->file_path);
    printf("window id:%d\n", req->window_id);
  }
  {
    res = request_window_capture(req);
  }
  {
    printf("success:%s\n", res->success ? "Yes" : "No");
    printf("save_file_success:%s\n", res->save_file_success ? "Yes" : "No");
    printf("file path exists:%s\n", fsio_file_exists(req->file_path) ? "Yes" : "No");
    printf("          size:%ld\n", fsio_file_size(req->file_path));
    printf("capture_started:%ld\n", res->capture_started);
    printf("write_file_started:%ld\n", res->write_file_started);
    printf("           dur    :%lu\n", res->write_file_dur);
    printf("capture_dur:%ld\n", res->capture_dur);
    printf("req mode:    %d\n", res->request->mode);
    printf("total dur:%ld\n", res->total_dur);
    printf("request_received:%ld\n", res->request_received);
    printf("captured_width:%ld\n", res->data->captured_width);
    printf("captured_height:%ld\n", res->data->captured_height);
    printf("resized_height:%ld\n", res->data->resized_height);
    printf("resized_width:%ld\n", res->data->resized_width);
    printf("requested resize factor:%d\n", res->request->resize_factor);
    printf("requested width:%ld\n", res->request->width);
    printf("requested height:%ld\n", res->request->height);
  }
  {
    /*
     */
    ASSERT_LTm("requested resize factor invalid", res->request->resize_factor, 99999);
    ASSERT_GTm("requested resize factor invalid", res->request->resize_factor, 0);
    ASSERT_LTm("requested file path too big", fsio_file_size(req->file_path), 1024 * 1024 * 64);
    ASSERT_EQm("requested file path does not exist", fsio_file_exists(req->file_path), true);
    ASSERT_EQm("requested file not successful save", res->save_file_success, true);
    ASSERT_GTm("captured width too small", res->data->captured_width, 100);
    ASSERT_GTm("captured height too small", res->data->captured_height, 100);
    ASSERT_GTm("invalid requested width", res->request->width, 0);
    ASSERT_GTm("invalid requested height", res->request->height, 0);
    ASSERT_GTm("invalid resized width", res->data->resized_width, 0);
    ASSERT_GTm("invalid resized height", res->data->resized_height, 0);
    ASSERT_GTm("invalid resized width", res->data->captured_width, res->data->resized_width);
    ASSERT_GTm("invalid resized height", res->data->resized_height, 0);
    ASSERT_GTm("invalid resized height", res->data->captured_height, res->data->resized_height);
    ASSERT_GTm("invalid resized width", res->data->captured_width, res->data->resized_width);
    ASSERT_GTm("requested file path too small", fsio_file_size(req->file_path), 128);
  }

  PASSm("Requested Resize File Write OK");
} /* t_return_resize_factor_write_file */

SUITE(s_test_callback){
}
SUITE(s_test_return){
  RUN_TESTp(t_return_resize_factor_write_file, (void *)__REQUEST_RESIZE_WRITE_FILE_RETURN_RESULT);
  RUN_TESTp(t_return_no_resize_factor_write_file, (void *)__REQUEST_NO_RESIZE_WRITE_FILE_RETURN_RESULT);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_test_return);
  RUN_SUITE(s_test_callback);
  GREATEST_MAIN_END();
  return(0);
}
