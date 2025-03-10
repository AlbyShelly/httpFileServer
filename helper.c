#include<stdio.h>
#include<string.h>

void getPath(char *req, char *path){
    
    int i = 4;
    int j = 0;

    while(str[i] != ' '){
        result[j++] = str[i++];
    }

    result[j] = '\0';

}

int main(){

    char str[256] = "GET /user/profile HTTP/1.1";
    char result[256];

    getPath(str, result);
    printf("Route: %s\n", result);

}
