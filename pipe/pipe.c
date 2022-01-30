#include <stdio.h>
#include <stdlib.h>

static int command_pipe_read;
static int command_pipe_write;

static int result_pipe_read;
static int result_pipe_write;

void func(int n) {

    if (n < 1) {
        return 1;
    }
    // sleep(0.001);
    printf("%d ", n);
    return func(n - 1);
}

void main() {

    int p[2];

    pid_t son;
    
    if(pipe(p) == -1){
        perror("pipe");
        exit(1);
    }

    command_pipe_read = p[0];
    command_pipe_write = p[1];

    if(pipe(p) == -1){
        perror("pipe");
        exit(1);
    }


    result_pipe_read = p[0];
    result_pipe_write = p[1];


    son = fork();

    if(son == -1) { //fork fail
        perror("fork");
        exit(1);
    }

    if(son) { // parent > 0
        printf("parent\n");
        close(result_pipe_write);
        close(command_pipe_read);
        return;
    }
    
    // son
    printf("son\n");
    close(result_pipe_read);
    close(command_pipe_write);

    // func()
    printf("123\n");
    // func(7);
    system("echo 123\n");

}