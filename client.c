
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
  char* host;
  int operation;
  struct request* request;
  int fd;
  char* file_in_content;
  uint8_t buff[MAXLINE+1];

  char* file_in = (char*)malloc(sizeof(char) * MAX_BUFF);
  char* input = (char*)malloc(sizeof(char) * MAX_BUFF);

  printf("enter operation\n0 :: WRITE\n1 :: READ\n");
  scanf("%d", &operation);

  if (operation < 0 || operation > 1) {
    err_n_die("operation error, invalid");
  }

  printf("enter filename :: ");
  scanf("%s", input);

  if (operation == WRITE) {
    printf("Enter file to send to server :: ");
    scanf("%s", file_in);
    if ((fd = open(file_in, O_RDONLY)) == -1)
      err_n_die("no such file\n");

    // read file specified to file_in_content
    int fd = open(input, O_RDONLY);
    if (fd == -1)
      err_n_die("no such file");
    file_in_content = read_file(buff, input, &fd);
    request = create_request(operation, strlen(input), 
        strlen(file_in_content), input, file_in_content);
  } else {
    request = create_request(operation, strlen(input),
        0, input, NULL);
  } 

  // error check input and file_in
  if (input == NULL || file_in == NULL) {
    free(input);
    free(file_in);
    err_n_die("input error");
  }

  // usage check
  if (argc > 1) {
    if (strncmp("local", argv[1], 5) == 0) {
      host = "127.0.0.1";
    } else {
      host = argv[1];
    }
  } else {
    host = "127.0.0.1";
  }

  char* request_str = request_to_str(*request);

  // create socket
  printf("using host :: %s on port %d\n", host, PORT);
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err_n_die("creation error");

  // create socket information
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if(inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0)
    err_n_die("address assignment error");

  // connect to server
  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    err_n_die("connection failed");

  // send data
  send(sock, request_str, strlen(request_str), 0);
  
  // read data
  valread = read(sock, buffer, 1024);

  // take response and parse to response struct
  char** args = split_line(buffer);
  struct response* response = parse_response(args);

  // close the socket when done
  close(sock);

  return 0;
}

