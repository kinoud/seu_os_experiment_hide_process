#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<assert.h>
#include<errno.h>

int main(int argc,char* argv[]){
    int uid=getuid();
    int pid=getpid();
    printf("now uid=%d pid=%d\n",uid,pid);
    printf("tap to hide");
    getchar();

    int flag=syscall(440,pid,1);

    if(flag==0)
        printf("hide ok\ntap to retrieve");
    else if(flag==-1){
        printf("permission denied\n");
        return 0;
    }
    getchar();

    syscall(440,pid,0);

    printf("retrieve ok\ntap to quit");
    getchar();
    printf("goodbye\n");
}
