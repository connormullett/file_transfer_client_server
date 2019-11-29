
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "common.h"

#define PORT 9001

int main(int argc, char* argv[]) {
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char file_name[MAXLINE+1];
  char* request_body;
  char buffer[1024] = {0};
  char* host;
  int request_type;

  if (argc > 1) {
    if (strncmp("local", argv[1], 5) == 0) {
      host = "127.0.0.1";
    } else {
      host = argv[1];
    }
  } else {
    host = "127.0.0.1";
  }

  printf("using host :: %s on port %d\n", host, PORT);

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err_n_die("creation error");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if(inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0)
    err_n_die("address assignment error");

  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    err_n_die("connection failed");

  request_body = "test_file\n";
  send(sock, request_body, strlen(request_body), 0);
  valread = read(sock, buffer, 1024);
  printf("%s\n", buffer);

  close(sock);

  return 0;
}

