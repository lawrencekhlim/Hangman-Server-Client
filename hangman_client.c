#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//Starter code taken from linuxhowtos.org

void write_to_server(int sockfd, char out){
  int n;
  char output[2];
  output[0]=1;
  output[1]=out;
  n = write(sockfd,output,2);
  if (n < 0)
       error("ERROR writing to socket");
}

char* read_from_server(int sockfd, char* incorrect, int* num_incorrect, int* done ){
  char buffer[255];
  bzero(buffer,255);
  int n = read(sockfd,buffer,255);
  if (n < 0)
       error("ERROR reading from socket");

  if(buffer[0] == 0){
    int msglen = buffer[1];
    *num_incorrect = buffer[2];
    char *str = malloc(msglen + 1);

    *done = 1;
    for(int i=0;i<msglen;i++){
      str[i] = buffer[i+3];
      if(str[i] == '_'){
        *done = 0;
      }
    }


    for(int i=0;i<(*num_incorrect);i++){
      incorrect[i] = buffer[i+msglen+3];
    }
    return str;
  }
  else{
    int msglen = buffer[0];
    char *str = malloc(msglen + 1);
    memset(str, 0, msglen+1);
    for(int i=0;i<msglen;i++){
      str[i] = buffer[i+1];
    }
    *done=1;
    return str;
  }
}


int handle_message_from_server(int sockfd) {
  char incorrect[6];
  int num_incorrect;
  int done=0;

  num_incorrect=-1;
  char* msg = read_from_server(sockfd,incorrect,&num_incorrect, &done);
  printf("%s\n",msg);
  if(num_incorrect == 6 || done == 1){
    return 1;
  }
  if(num_incorrect!=-1){
    printf("Incorrect Guesses: ");
    for(int i=0;i<num_incorrect;i++){
      printf("%c", incorrect[i]);
    }
    printf("\n");
  }

  return 0;
}


int main(int argc, char *argv[])
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");



    printf("Ready to start game? (y/n):");
    char start[64];
    fgets(start,64,stdin);
    if(start[0] == 'n'){
      return 0;
    }

    //TODO:check if this works
    char out [1];
    out[0]='\0';
    int n = write(sockfd,out,1);
    if (n < 0)
         error("ERROR writing to socket");



    int done = handle_message_from_server(sockfd);
    while(done == 0){

      char input[64];
      do {
        printf("Letter to guess: ");
        fgets(input,64,stdin);
        if(strlen(input) != 2){
          printf("Error! Please guess one letter.");
        }
      } while(strlen(input) != 2);

      write_to_server(sockfd, tolower(input[0]));
      done = handle_message_from_server(sockfd);
    }




    close(sockfd);
    return 0;
}
