//
// Created by semen on 22.04.19.
//


#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {

    char foo[] = "This is still text from process-1";

    printf("Now execute\n");
    printf("  sudo ./memory_read  %d  %lx  %lu\n",
           getpid(),
           (long unsigned int) foo,
           strlen(foo) + 1);

    printf("Press any key\n");
    getchar();

    printf("foo has changed to: %s\n", foo);
    return 0;
}
