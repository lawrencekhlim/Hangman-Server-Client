// Dsed sample codes from October 12th discussion section.
// Lawrence Lim
// Perm: 4560892

/* A simple server in the internet domain using TCP
 The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


// recursively add together digits and send.
void recurse_addition (int sockfd, char buffer [], int buffer_length) {
    
    if (buffer_length == 1) {
        int n = write(sockfd, buffer, 1);
        if (n < 0) error ("sendto");
    }
    else
        if (buffer_length > 1) {
            int total = 0;
            for (int i = 0; i < buffer_length; i++) {
                total+= buffer[i] -48;
            }
            char new_buffer [128];
            bzero (new_buffer, 128);
            sprintf (new_buffer, "%d", total);
            int count = 0;
            count = strlen (new_buffer);
            
            if (count > 1) {
                int n = write (sockfd, new_buffer, 128);
                if (n < 0) error ("sendto");
            }
            recurse_addition (sockfd, new_buffer, count);
        }
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buf[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int on = 0;
    setsockopt(sockfd, SOCK_RDM, TCP_NODELAY, (const char *)&on, sizeof(int));
    if (sockfd < 0)
        error("ERROR opening socket");
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr,  sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    while (1) {
        
        // Listen for incoming connections
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        int on = 0;
        setsockopt(newsockfd, SOL_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
        if (newsockfd < 0)
            error("ERROR on accept");
        
        bzero(buf,256);
        n = read(newsockfd,buf,255);
        if (n < 0) error("ERROR reading from socket");
        
        // Check that string is all number characters
        int count = n;
        int non_num = 0;
        int ignore_last = 0;
        for (int i = 0; i < count; i++) {
            if (i != count-1 && (buf [i] < '0' || buf [i] > '9'))
                non_num = 1;
            else if (i == count-1) {
                if (buf [i] < '0' || buf [i] > '9')
                    ignore_last = 1;
            }
        }
        
        if (non_num) {
            n = write(newsockfd,"Sorry, cannot compute!", 25);
        }
        else {
            char newstr [1024];
            strcpy(newstr, buf);
            if (ignore_last) {
                count = count-1;
                newstr [count] = '\0';
            }
            recurse_addition (newsockfd, newstr, count);
        }
        if (n  < 0) error("sendto");
        
        close(newsockfd);
        
    }
    close(sockfd);
    return 0;
}

