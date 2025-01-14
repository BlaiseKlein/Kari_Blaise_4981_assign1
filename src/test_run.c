//
// Created by blaise-klein on 1/13/25.
//
// TODO DELETE THIS FILE BEFORE HANDIN

#include "http.h"
#include "network.h"

int main(void)
{
    char   *buffer;
    ssize_t msg_size = 4;
    int     err      = 0;
    int     fd       = open("./test.txt", O_RDWR | O_CLOEXEC);
    if(fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    buffer = (char *)malloc((size_t)msg_size * sizeof(char));
    if(buffer == NULL)
    {
        perror("malloc");
        close(fd);
        exit(EXIT_FAILURE);
    }

    buffer[0] = 't';
    buffer[1] = 'e';
    buffer[2] = 's';
    buffer[3] = 't';

    // write_fully(fd, buffer, msg_size, &err);

    read_fully(fd, buffer, msg_size, &err);

    if(err == 0)
    {
        printf("read: %c%c%c%c\n", buffer[0], buffer[1], buffer[2], buffer[3]);
    }

    free(buffer);
    close(fd);

    return 0;
}