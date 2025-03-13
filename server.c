#include<fcntl.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>

#include"helper.c"

void getPath(char *req, char* path);

int main(int argc, char* argv[]){

    if(argc != 2){
        printf("Usage: ./server portno.");
        exit(1);
    }
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(sockfd < 0){
        perror("Error opening socket");
        exit(1);
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[1])); //set port number
    servAddr.sin_addr.s_addr = INADDR_ANY; //set host ip

    if(bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
        perror("Binding failed");
        exit(1);
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
        getPath(buff, path);
        printf("Path: %s\n", path);

        if(strcmp(path, "/") == 0){
            n = writeDirectoryListing(newsockfd);
            
            if(n < 0)
                exit(1);

        }else{
            n = writeFile(newsockfd, path);

            if(n < 0)
                exit(1);
        }

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
    
    
    
