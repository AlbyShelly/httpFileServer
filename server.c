#include<fcntl.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>

void getPath(char *req, char* path);

int main(int argc, char* argv[]){

    if(argc != 2){
        printf("Usage: ./server portno.");
        exit(1);
    }
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(sockfd < 0){
        perror("Error opening socket");
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[1])); //set port number
    servAddr.sin_addr.s_addr = INADDR_ANY; //set host ip

    if(bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
        perror("Binding failed");
    }

    if(listen(sockfd, 5) < 0)
        perror("listen failed");

    printf("Server: started successfully\n");

    struct sockaddr_in cltAddr;
    socklen_t cltAddrlen = sizeof(cltAddr);

    for(;;){
        int newsockfd;

        do{
            newsockfd = accept(sockfd, (struct sockaddr *)&cltAddr, &cltAddrlen);

            if(newsockfd < 0)
                perror("Connection failed retrying...");

        }while(newsockfd < 0);

        printf("Server: client connected successfully\n");

        char buff[1024];
        char path[256];

        int n = read(newsockfd, buff, sizeof(buff));
        printf("Request: %s\n", buff);
        getPath(buff, path);
        printf("Path: %s\n", path);

        bzero(buff, sizeof(buff));
        sprintf(buff,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %d\r\n\r\n"
                "<html>%s</html>",
                strlen(path) + 13,
                path
        );

        printf("Response: %s\n", buff);

        n = write(newsockfd, buff, strlen(buff)); 

        if(n < 0)
            perror("Error on write");

        close(newsockfd);
    }

    return 0;
}


void getPath(char *req, char *path){

    int i = 4;
    int j = 0;

    while(req[i] != ' '){
        path[j++] = req[i++];
    }

    path[j] = '\0';
}    
    
    
    
