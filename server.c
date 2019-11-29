
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
    connfd = accept(listenfd, (SA*) NULL, NULL);

    memset(recvline, 0, MAXLINE);

    while( (n = read(connfd, recvline, MAXLINE-1)) > 0) {
      filename = (char*)recvline;
      if (recvline[n-1] == '\n')
        break;
      memset(recvline, 0, MAXLINE);
    }

    if (n < 0)
      err_n_die("read error");

    // open file for reading
    fd = open(filename, O_RDONLY); // TODO: need error checking

    // get filesize from request
    struct stat st;
    fstat(fd, &st);
    int file_size = st.st_size;

    // read file
    read(fd, buff, file_size);
    printf("%s\n", buff);

    // close file
    close(fd);

    // write to connection socket and close
    write(connfd, (char*)buff, strlen((char*)buff));
    close(connfd);
  }
}

