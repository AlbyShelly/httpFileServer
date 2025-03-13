#include<dirent.h>
#include<stdio.h>
#include<strings.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

int writeDirectoryListing(int sockfd){
    
    //calculate the body size
    DIR *dirp = opendir(".");
    struct dirent *entry;
    
    if(dirp == NULL){
       perror("Error opening directory");
       return -1;
    } 
    
    char buffbody[1024]; 
    int n = sprintf(buffbody, "<html><ul>");

    while((entry = readdir(dirp)) != NULL){
        n += sprintf(buffbody + n, "<li><a href=\"/%s\">%s</a></li>", entry->d_name, entry->d_name);
    }

    n += sprintf(buffbody + n, "</ul></html>");

    char buffheader[1024];
    write(sockfd, 
          buffheader,
          sprintf(buffheader,
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html\r\n"
                  "Content-Length: %d\r\n\r\n",
                  n
          )
    );

    n = write(sockfd, buffbody, n);

    if(n < 0){
        perror("Error on write");
        return -1;
    }
    
    printf("%s\n", buffheader);

    return 0;
}

int writeFile(int sockfd, char *file){
    
    printf("Path: %s\n", file + 1);
    char buff[1024] = "HTTP/1.1 200 OK\r\n"
                      "Content-Disposition: attachment\r\n"
                      "filename=\"%s\"\r\n"
                      "Content-Length: %d\r\n\r\n";
    
    //get file size
    struct stat st;

    if(stat(file + 1, &st) < 0){
        perror("Error reading file attr\n");
        return -1;
    }

    //write header
    int n = sprintf( buff + 512, buff, file + 1, st.st_size);
    
    if(write(sockfd, buff + 512, n) < 0){
        perror("Error on write");
        return -1;
    }

    printf("%s\n", buff + 512);


    //open and write file
    FILE *fptr = fopen(file + 1, "r"); //truncate / in file name

    if(fptr == NULL){
        perror("Error on file open");
        return -1;
    }

    bzero(buff, sizeof(buff));

    while((n = fread(buff, 1,  sizeof(buff), fptr)) != 0){
        
        if(write(sockfd, buff, n) < 0){
            perror("Error on write");;
            return -1;
        }

    }

    return 0;
}

