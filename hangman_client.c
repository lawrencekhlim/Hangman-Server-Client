// Used sample code from October 12th discussion section
// Lawrence Lim
// Perm: 4560892
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    char buffer[129];
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
    printf("Enter message: ");
    bzero(buffer,129);
    fgets(buffer,129,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");
    
    
    char buf [128];
    int z = 0;
    do {
        bzero (buf, 128);
        z = read (sockfd, buf, 128);
        char out [128];
        bzero (out, 128);
        strcat (out, "Server: ");
        strcat (out, buf);
        strcat (out, "\n");
        write (1, out, strlen (out));
        
    } while (z > 1 && strcmp(buf, "Sorry, cannot compute!") != 0);
    
    /*
     char buf [1024];
     bzero (buf, 1024);
     n = read (sockfd, buf, 1024);
     if (n < 0)
     error("ERROR reading from socket");
     //printf("Server: %s\n",buffer);
     printf (buf);
     int i = 0;
     do {
     while (buf[i] != ' ') {
     char c[1];
     c[0] = buf[i];
     printf (c);
     i++;
     }
     i++;
     if (buf[i] != '\0') {
     printf ("Server: ");
     }
     } while (i < n);
     */
    close(sockfd);
    return 0;
}

