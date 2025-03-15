#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>

int main(){

    struct stat sb;
    stat("a.out", &sb);

    if((sb.st_mode & S_IFMT) == S_IFDIR){
        printf("is a directory\n");
    }else{
        printf("Not a directory\n");
    }
}
