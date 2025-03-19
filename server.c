#include<arpa/inet.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>

#include"helper.c"

void getPath(char *req, char* path);
void sigpipeHandler(int n);

int main(int argc, char* argv[]){
    
    //ignore SIGPIPE
    signal(SIGPIPE, sigpipeHandler);

    if(argc != 2){
        printf("Usage: ./server portno.\n");
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

    printf("Server: Started running on port %d ip %s\n", ntohs(servAddr.sin_port), inet_ntoa(servAddr.sin_addr));

    struct sockaddr_in cltAddr;
    socklen_t cltAddrlen = sizeof(cltAddr);

    for(;;){
        printf("\n\n");

        int newsockfd;

        do{
            newsockfd = accept(sockfd, (struct sockaddr *)&cltAddr, &cltAddrlen);

            if(newsockfd < 0)
                perror("Connection failed retrying...");

        }while(newsockfd < 0);

        printf("Server: client connected port %d ip %s\n", ntohs(cltAddr.sin_port), inet_ntoa(cltAddr.sin_addr));

        char buff[1024];
        char path[256];

        int n = read(newsockfd, buff, sizeof(buff));

        printf("Server: bytes read %d \n", n);
        
        printf("Request: ");
        
        for(int i = 0; buff[i] != '\r'; i++)
            printf("%c", buff[i]);
        
        printf("\n");

        getPath(buff, path);

        if(strcmp(path, "/") == 0){

            if(writeDirectoryListing(newsockfd, ".") < 0)
                printf("Server: No response\n");

        }else{

            //check route if file or dir
            struct stat sb;

            //file not exsist    
            if(stat(path+1, &sb) < 0){

                if(writeFileNotFound(newsockfd) < 0){
                    printf("Server: No response\n");
                }

            }else if((sb.st_mode & S_IFMT) == S_IFDIR){
                
                if(writeDirectoryListing(newsockfd, path+1) < 0){
                    printf("Server: No response\n");
                }

            }else{

                if(writeFile(newsockfd, path) < 0){
                    printf("Server: No response\n");
                }
            }
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
    
void sigpipeHandler(int n){
    printf("SIGPIPE caught\n");
}

