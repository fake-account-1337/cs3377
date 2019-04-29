#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#define MAXLINE 4096 /*max text line length*/
//#define SERV_PORT 10010 /*port*/

int
main(int argc, char **argv) 
{
  int sockfd;
  struct sockaddr_in servaddr;
  char sendline[MAXLINE], recvline[MAXLINE];
  time_t now;
  FILE *commandFile = fopen(argv[3], "r");
   
  //basic check of the arguments
  if (argc !=4) {
    perror("Usage: TCPClient <Server IP> <Server Port> <Command file>"); 
    exit(1);
  }
   
  //Create a socket for the client
  //If sockfd<0 there was an error in the creation of the socket
  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
    perror("Problem in creating the socket");
    exit(2);
  }
   
  //Creation of the socket
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  inet_aton(argv[1], &servaddr.sin_addr);
  servaddr.sin_port = htons(atoi(argv[2])); 
  //Connection of the client to the socket 
  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
    perror("Problem in connecting to the server");
    exit(3);
  }
  time(&now);
  printf("%s %d:%s", "Client", getpid(), ctime(&now)); // Client <PID>:<DATE AND TIME>
  while (fgets(sendline, MAXLINE, commandFile) != NULL) {
    send(sockfd, sendline, strlen(sendline), 0); // Send command read from command file to server
    int i;
    if(recv(sockfd, recvline, MAXLINE,0) == 0) {  //error: server terminated prematurely
      perror("The server terminated prematurely"); 
      exit(4);
    }
    time(&now);
    printf("%s %d:%s", "Client", getpid(), ctime(&now));
    printf("%s\n", recvline);
  }
  fclose(commandFile);
  exit(0);
}
