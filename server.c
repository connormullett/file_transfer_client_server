
#include "common.h"
#include <strings.h>


int main(int argc, char** argv) {
  int listenfd, connfd, n, request_type;
  unsigned int recieved_length;
  struct sockaddr_in servaddr;
  uint8_t buff[MAXLINE+1];
  uint8_t recvline[MAXLINE+1];
  char* filename;
  int fd;
  int servaddr_len = sizeof(servaddr);
  char* inc_request;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err_n_die("socket error");

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERVER_PORT);

  if ((bind(listenfd, (SA*)&servaddr, sizeof(servaddr))) < 0)
    err_n_die("bind error");

  if((listen(listenfd, 10)) < 0)
    err_n_die("listen error");

  for( ; ; ) {
    struct sockaddr_in addr;
    socklen_t addr_len;
    printf("waiting for connection on port %d\n", SERVER_PORT);
    fflush(stdout);
    connfd = accept(listenfd, (SA*)&servaddr, (socklen_t*)&servaddr_len);
    printf("Connection Recieved :: host %s\n",
        inet_ntoa(servaddr.sin_addr));

    memset(recvline, 0, MAXLINE);

    n = read(connfd, recvline, MAXLINE-1);
    filename = (char*)recvline;
    printf("bytes read (n) :: %d\n", n);
    printf("request :: %s\n", filename);

    printf("requesting file %s from %s\n",
        filename,
        inet_ntoa(servaddr.sin_addr));

    if (n < 0)
      err_n_die("read error");

    // open file for reading
    fd = open(filename, O_RDONLY); // TODO: need error checking

    if (fd < 0) {
      printf("no such file :: %s\n", filename);
      send_error_response(connfd);
    } else {
      char* file_content = read_file(buff, filename, &fd);
      printf("content :: %s\n", file_content);
      write(connfd, file_content, strlen(file_content));
    }

    close(connfd);
  }

  close(listenfd);
}

