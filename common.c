
#include "common.h"

void err_n_die(const char* fmt, ...) {
  int errno_save;
  va_list ap;

  errno_save = errno;

  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  fprintf(stdout, "\n");
  fflush(stdout);

  if (errno_save != 0) {
    fprintf(stdout, "(errno = %d) : %s\n", errno_save,
        strerror(errno_save));
    fprintf(stdout, "\n");
    fflush(stdout);
  }
  va_end(ap);

  exit(1);
}


char* bin2hex(const unsigned char* input, size_t len) {
  char* result;
  char* hexits = "0123456789ABCDEF";

  if (input == NULL || len <= 0)
    return NULL;

  int resultlength = (len*3) + 1;
  result = malloc(resultlength);
  bzero(result, resultlength);

  for (int i = 0; i < len; i++) {
    result[i * 3] = hexits[input[i] >> 4];
    result[(i * 3)+1] = hexits[input[i] & 0x0F];
    result[(i * 3) + 2] = ' ';
  }

  return result;
}


void send_error_response(int fd) {
  char* response = "-1";

  write(fd, response, strlen(response));
  err_n_die("file opening error");
}


char* read_file(uint8_t* buff, const char* filename, int* fd) {
  struct stat st;
  int file_size;

  // get file's size
  fstat(*fd, &st);
  file_size = st.st_size;

  // read file
  read(*fd, buff, file_size);

  // close file
  close(*fd);

  return (char*)buff;
}


struct response* parse_response(char** args) {
  int status = atoi(args[0]);
  int body_length = atoi(args[1]);
  char* body = (char*)malloc((sizeof(char) * body_length) + 1);
  struct response* out = (struct response*)malloc(sizeof(struct response));
  out->status = status;
  out->body_length = body_length;
  out->body = body;
  return out;
}


struct response* create_response(int status, int body_length, char* body) {
  struct response* out = (struct response*)malloc(sizeof(struct response));
  out->status = status;
  out->body_length = body_length;
  char* h_body = (char*)malloc((sizeof(char) * body_length) + 1);
  out->body = h_body;
  return out;
}


char* response_to_str(struct response* response) {
  size_t len = 0;
  len = snprintf(NULL, len, "%d\n%d\n%s\n", response->status,
      response->body_length, response->body);

  char *response_str = calloc(1, sizeof(*response_str) * len + 1);
  if (!response_str) {
    fprintf(stderr, "%s() error: memory allocation failed\n", __func__);
  }

  if (snprintf(response_str, len + 1, "%d\n%d\n%s\n", response->status,
        response->body_length, response->body) > len + 1) {
    fprintf (stderr, "%s()  error: snprintf truncated result\n", __func__);
    return NULL;
  }

  return response_str;
}


struct request* parse_request(char** args) {
  int operation = atoi(args[0]);
  int len_filename = atoi(args[1]);
  int len_content = atoi(args[2]);
  char* filename = (char*)malloc((sizeof(char) * len_filename) + 1);
  char* content = (char*)malloc((sizeof(char) * len_content) + 1);

  struct request* out = (struct request*)malloc(sizeof(struct request));
  out->operation = operation;
  out->len_filename = len_filename;
  out->len_content = len_content;
  out->filename = filename;
  out->content = content;
  return out;
}


struct request* create_request(int operation, int len_filename, int len_content,
    char* filename, char* content) {
  struct request* out = (struct request*)malloc(sizeof(struct request));
  out->operation = operation;
  out->len_filename = len_filename;
  out->len_content = len_content;
  out->filename = filename;
  out->content = content;
  return out;
}


char* request_to_str(struct request* request) {
  size_t len = 0;
  len = snprintf(NULL, len, "%d\n%d\n%d\n%s\n%s\n", request->operation,
      request->len_filename, request->len_content, request->filename,
      request->content);

  char *request_str = calloc(1, sizeof(*request_str) * len + 1);
  if (!request_str) {
    fprintf(stderr, "%s() error: memory allocation failed\n", __func__);
  }

  if (snprintf(request_str, len + 1, "%d\n%d\n%d\n%s\n%s\n", request->operation,
      request->len_filename, request->len_content, request->filename,
      request->content)) {
    fprintf (stderr, "%s()  error: snprintf truncated result\n", __func__);
    return NULL;
  }

  return request_str;
}

