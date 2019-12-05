
#include "common.h"
#include <strings.h>

int listenfd;

void interrupt_handler(int num){
  close(listenfd);
  err_n_die("\nrecieved SIGINT\n");
}


int main(int argc, char** argv) {
  int connfd, n, request_type;
  unsigned int recieved_length;
  struct sockaddr_in servaddr;
  uint8_t buff[MAXLINE+1];
  uint8_t recvline[MAXLINE+1];
  int fd;
  int servaddr_len = sizeof(servaddr);
  char* filename;
  char** args;
  struct request* request;

  // handle sigint
  signal(SIGINT, interrupt_handler);

  // usage check
  if (argc != 3) {
    printf("usage %s <host> <port number>\n", __FILE__);
    exit(EXIT_FAILURE);
  }

  char* host = argv[1];
  int port = atoi(argv[2]);

  // create socket for listening
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err_n_die("socket error");

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if ((bind(listenfd, (SA*)&servaddr, sizeof(servaddr))) < 0)
    err_n_die("bind error");

  if((listen(listenfd, 10)) < 0)
    err_n_die("listen error");

  for( ; ; ) {
    struct sockaddr_in addr;
    socklen_t addr_len;
    printf("waiting for connection on port %d\n", port);
    fflush(stdout);
    connfd = accept(listenfd, (SA*)&servaddr, (socklen_t*)&servaddr_len);
    printf("Connection Recieved :: host %s\n",
      inet_ntoa(servaddr.sin_addr));

    memset(recvline, 0, MAXLINE);

    // read from connection socket
    n = read(connfd, recvline, MAXLINE-1);
    if (n < 0)
      err_n_die("read error");

    // parse args from read buffer
    args = split_line((char*)recvline);
    request = parse_request(args);

    // log request
    printf("requesting file %s from %s\n",
      request->filename,
      inet_ntoa(servaddr.sin_addr));

    // check if operation is for read or write
    if (request->operation == READ) {
      // open file for reading
      fd = open(request->filename, O_RDONLY);

      if (fd < 0) {
        printf("no such file :: %s\n", request->filename);
        send_error_response(connfd);
      } else {
        char* file_content = read_file(buff, request->filename, &fd);
        write(connfd, file_content, strlen(file_content));
      }
    } else if (request->operation == WRITE){
      fd = open(request->filename, O_CREAT);
      write(fd, request->content, request->len_content);
      send_success_response(connfd);
    } else {
      // invalid operation
      // TODO: fix for using response struct
      send_error_response(connfd);
    }

    close(connfd);
  }
}

