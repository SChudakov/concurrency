#include <linux/input.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
// sudo ./keystroke_capture /dev/input/event4
int main(int argc, char **argv) {
    int fd;
    if (argc < 2) {
        printf("usage: %s <device>\n", argv[0]);
        return 1;
    }

    char *device = argv[1];
    printf("device: %s\n", device);
    fflush(stdout);

    fd = open(device, O_RDONLY);
    printf("fd: %d", fd);
    fflush(stdout);

    struct input_event event;

    while (1) {
        read(fd, &event, sizeof(struct input_event));

        if (event.type == 1) {
            if (event.value == 1) {
                printf("key %i pressed\n", event.code);
            } else if (event.value == 0) {
                printf("key %i unpressed\n", event.code);
            }
            fflush(stdout);
        }
    }
    return 0;
}