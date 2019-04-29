#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/

int main (int argc, char **argv)
{
  int listenfd, connfd, n;
  pid_t childpid;
  socklen_t clilen;
  char buf[MAXLINE];
  struct sockaddr_in cliaddr, servaddr;
  FILE *fp;
  char path[MAXLINE];
  int status;
  time_t now;

  //Create a socket for the socket
  //If sockfd<0 there was an error in the creation of the socket
  if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
    perror("Problem in creating the socket");
    exit(2);
  }

  //preparation of the socket address
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));

  //bind the socket
  bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

  //listen to the socket by creating a connection queue, then wait for clients
  listen (listenfd, LISTENQ);

  printf("%s\n","Server running...waiting for connections.");

  for ( ; ; ) {
    clilen = sizeof(cliaddr);
    //accept a connection
    connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

    if ( (childpid = fork ()) == 0 ) {  //if it’s 0, it’s child process
      //close listening socket
      close (listenfd);

      while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
        time(&now);
        printf("%s %d:%s","Server", getpid(), ctime(&now)); // Server <PID>:<DATE AND TIME>
        puts(buf);
        if (strcmp(buf, "end\n") == 0){ // End of connection with a client
          send(connfd, "Ending transmission...\n", MAXLINE, 0);
          exit(0);
        }
        fp = popen(buf, "r"); // Use popen to run command read from connection
        char sendToClient[MAXLINE]; 
        while( fgets(path, MAXLINE, fp) != NULL ) {
          strcat(sendToClient, path); // Aggregate output from popen
        }
        send(connfd, sendToClient, MAXLINE, 0);
        status = pclose(fp);
        if ( status == -1)
          printf("%s\n", "Error from pclose");
        int i;
        // Clear char array for buf and sendToClient for next transaction
        for (i = 0; i < MAXLINE; i++) {
          buf[i] = '\0';
          sendToClient[i] = '\0';
        }
      }
      exit(0);
    }
    //close socket of the server
    close(connfd);
  }
} 
