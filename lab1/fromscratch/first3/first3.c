#include <string.h>
#include <stdio.h>

int main(int argc, char** argv) {

    if(argc!=3) {
        printf("usage => %s <arg1> <arg2>\n",argv[0]);
        return 0;
    }

    if(strlen(argv[1])<4 || strlen(argv[2])<4) {
        printf("error: one or more arguments has less than 3 characters\n");
        return 0;
    }

    for(int i=0;i<3;i++) {
        printf("%c",argv[1][i]);
    }
    for(int i=0;i<3;i++) {
        printf("%c",argv[2][i]);
    }
    printf("\n");

    return 0;
}