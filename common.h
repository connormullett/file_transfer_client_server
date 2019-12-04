
#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

#define MAXLINE 4096
#define SA struct sockaddr

#define READ  1
#define WRITE 0

struct response {
  int status;
  int body_length;
  char* body;
};

struct request {
  int operation;
  int len_filename;
  int len_content;
  char* filename;
  char* content;
};

void err_n_die(const char* fmt, ...);
char* bin2hex(const unsigned char* input, size_t len);

void send_success_response(int fd);
void send_error_response(int fd);
char* read_file(uint8_t* buff, const char* filename, int* fd);

struct response* parse_response(char** args);
struct response* create_response(int status, int body_length, char* body);
char* response_to_str(struct response* response);

struct request* parse_request(char** args);
struct request* create_request(int operation, int len_filename, int len_content,
    char* filename, char* content);
char* request_to_str(struct request request);

char** split_line(char* input);

#endif

