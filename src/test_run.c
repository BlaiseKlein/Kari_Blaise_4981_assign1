//
// Created by blaise-klein on 1/13/25.
//
// TODO DELETE THIS FILE BEFORE HANDIN

#include "http.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #define TESTBUFFER 2048
#define TESTLEN 30

int main(void)
{
    // TEST 1
    //  struct thread_state *thread_state;
    //  const char          *http = "GET /en-US/docs/Web/HTTP/Messages HTTP/1.1\r\n";
    //  //                                "Host: example.com\r\n"
    //  //                                "Content-Type: application/x-www-form-urlencoded\r\n"
    //  //                                "Content-Length: 50\r\n";
    //
    //  thread_state = (struct thread_state *)malloc(sizeof(struct thread_state));
    //  if(thread_state == NULL)
    //  {
    //      perror("malloc");
    //      exit(EXIT_FAILURE);
    //  }
    //
    //  thread_state->request_line_string = (char *)malloc(strlen(http) + 1);
    //  if(thread_state->request_line_string == NULL)
    //  {
    //      perror("malloc");
    //      exit(EXIT_FAILURE);
    //  }
    //  thread_state->request_line_string_len = strlen(http);
    //  strlcpy(thread_state->request_line_string, http, thread_state->request_line_string_len);
    //  parse_request_line(thread_state);
    //  free(thread_state->request_line_string);

    // TEST2
    // char   buf[TESTBUFFER];
    // size_t result = 0;
    // int    err    = 0;
    // int    fd     = open("../random.txt", O_RDONLY | O_CLOEXEC);
    // if(fd == -1)
    // {
    //     perror("open");
    //     exit(1);
    // }
    //
    // result = read_until(fd, buf, TESTBUFFER, "|", &err);
    // if(err < 0 || result == 0)
    // {
    //     close(fd);
    //     perror("read");
    //     exit(1);
    // }
    // printf("%s\n", buf);
    // close(fd);

    // TEST3
    int                  fd;
    struct thread_state *data;
    int                  result = 0;
    fd                          = open("../random.txt", O_RDWR | O_CLOEXEC);
    if(fd == -1)
    {
        perror("open");
        exit(1);
    }

    write(fd, "From: Me\r\nContent-Length: 50\r\n", TESTLEN);
    close(fd);
    fd = open("../random.txt", O_RDWR | O_CLOEXEC);
    if(fd == -1)
    {
        perror("open");
        exit(1);
    }

    data = (struct thread_state *)malloc(sizeof(struct thread_state));
    if(data == NULL)
    {
        perror("malloc");
        exit(1);
    }

    data->client_fd = fd;
    data->err       = 0;

    result = parse_request_headers(data);
    if(data->err < 0 || result == 0)
    {
        close(fd);
        perror("read");
        exit(1);
    }
    close(fd);

    return 0;
}
