
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "common.h"

#define PORT 9001
#define MAX_BUFF 32

int main(int argc, char* argv[]) {
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char file_name[MAXLINE+1];
  char buffer[1024] = {0};
  struct request* request;
  int fd;

  // usage check
  if (argc != 3) {
    printf("usage %s <host> <port number>\n", __FILE__);
    exit(EXIT_FAILURE);
  }

  char* host = argv[1];
  int port = atoi(argv[2]);

  // this is a lot of heap allocation, fix
  char* file_in = (char*)malloc(sizeof(char) * MAX_BUFF);
  char* input = (char*)malloc(sizeof(char) * MAX_BUFF);

  printf("enter filename :: ");
  scanf("%s", input);

  request = create_request(strlen(input), input);

  // error check input and file_in
  if (input == NULL || file_in == NULL) {
    free(input);
    free(file_in);
    err_n_die("input error");
  }

  char* request_str = request_to_str(*request);

  // create socket
  printf("using host :: %s on port %d\n", host, port);
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err_n_die("creation error");

  // create socket information
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if(inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0)
    err_n_die("address assignment error");

  // connect to server
  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    err_n_die("connection failed");

  // send data
  printf("request_str :: %s\n", request_str);
  send(sock, request_str, strlen(request_str), 0);

  // read data
  valread = read(sock, buffer, 1024);

  // take response and parse to response struct
  char** args = split_line(buffer);
  // struct response* response = parse_response(args);

  // close the socket when done
  close(sock);

  return 0;
}

