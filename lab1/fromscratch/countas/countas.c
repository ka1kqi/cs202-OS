#include <string.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if(argc!=2) {
        printf("usage: %s <arg>\n",argv[0]);
        return 0;
    }

    char* s=argv[1];

    int n=0;
    
    while(*s!='\0') {
        if(*s=='a')
            n++;
        s++;
    }

    
    printf("%d 'a' characters in given argument\n",n);
    return 0;
}