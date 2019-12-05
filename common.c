
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


void send_success_response(int fd) {
  char* response = "0";

  write(fd, response, strlen(response));
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
  int len_filename = atoi(args[1]);
  char* filename = (char*)malloc((sizeof(char) * len_filename) + 1);

  struct request* out = (struct request*)malloc(sizeof(struct request));
  out->len_filename = len_filename;
  out->filename = filename;
  return out;
}


struct request* create_request(int len_filename, char* filename) {
  struct request* out = (struct request*)malloc(sizeof(struct request));
  out->filename = (char*)malloc(sizeof(char) * len_filename + 1);
  out->len_filename = len_filename;
  out->filename = filename;
  return out;
}


char* request_to_str(struct request request) {
  size_t len = 0;

  len = snprintf(NULL, len, "%d\n%s\n",
      request.len_filename, request.filename);

  char *request_str = calloc(1, sizeof *request_str  * len + 1);
  if (!request_str) {
    fprintf(stderr, "%s() error: memory allocation failed\n", __func__);
    err_n_die("struct parse error");
  }

  if (snprintf(request_str, len + 1, "%d\n%s\n",
      request.len_filename, request.filename) > len + 1) {
    fprintf (stderr, "%s()  error: snprintf truncated result\n", __func__);
    err_n_die("struct parse error");
  }

  return request_str;
}


char** split_line(char* input) {
  int bufsize = 64;
  int position = 0;
  char** tokens = malloc(bufsize * sizeof(char*));
  char* token;
  char delim[] = "\n";

  if(!tokens) {
    err_n_die("allocation error in split_line\n");
  }

  token = strtok(input, delim);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += bufsize;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens)
        err_n_die("allocation error in split_line\n");
    }

    token = strtok(NULL, delim);
  }

  tokens[position] = NULL;
  return tokens;
}

