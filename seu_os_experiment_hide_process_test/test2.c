#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<assert.h>
#include<errno.h>

char name[50]="test2";

int main(int argc,char* argv[]){
    int uid=getuid();
    int pid=getpid();
    printf("now uid=%d pid=%d\n",uid,pid);

    printf("tap to hide this process");
    getchar();

    syscall(441,uid,name);

    printf("hide ok\ntap to hide all process of user %d",uid);
    getchar();

    syscall(441,uid,NULL);

    printf("hide ok\n");
    printf("goodbye\n");
}
