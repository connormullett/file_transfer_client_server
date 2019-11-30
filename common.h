
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

#define SERVER_PORT 9001

#define MAXLINE 4096
#define SA struct sockaddr

#define READ  1
#define WRITE 0

void err_n_die(const char* fmt, ...);
char* bin2hex(const unsigned char* input, size_t len);
void send_error_response(int fd);
char* read_file(uint8_t* buff, const char* filename, int* fd);

#endif

