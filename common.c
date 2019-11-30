
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

