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
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}


int check_processes (int &pids [], int num_alive) {
    for (int i = num_alive-1; i >= 0; i--) {
        int status;
        pid_t result = waitpid (pids[i], &status, WNOHANG);
        if (result > 0) {
            pids [i] = pids [num_alive-1];
            pids [num_alive-1] = 0;
            num_alive --;
        }
    }
    return num_alive;
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
    
    int pids [4];
    pids [0] = 0;
    pids [1] = 0;
    pids [2] = 0;
    pids [3] = 0;
    
    int num_connections = 0;
    while (1) {
        // Check for closed connections here
        num_connections = check_processes (pids, num_connections);
        
        if (num_connections < 4) {
            // Wait and listen for incoming connections
            ewsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        
            // AND Check for closed connections here
            num_connections = check_processes (pids, num_connections);
            
            pid = fork();
            if (pid == 0 && num_connections < 3) {
                // If there are still connections possible, I am the child process
                int on = 0;
                setsockopt(newsockfd, SOL_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
                if (newsockfd < 0)
                    error("ERROR on accept");
            
        
        
                close(newsockfd);
                
                return 0;
            }
            else if (pid == 0 && num_connections >= 3) {
                // If no connections are available and I am a child process
                
                int on = 0;
                setsockopt(newsockfd, SOL_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
                if (newsockfd < 0)
                    error("ERROR on accept");
                
                
                
                close(newsockfd);
            
                return 0;
            }
            else {
                // If I am the parent process
                pids[num_connections] = pid;
                num_connections++;
            }
        }
        
    }
    close(sockfd);
    return 0;
}



