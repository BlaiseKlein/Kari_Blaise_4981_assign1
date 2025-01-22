//
// Created by blaise-klein on 1/13/25.
//
// TODO DELETE THIS FILE BEFORE HANDIN

#include "http.h"
#include <stdio.h>

int main(void)
{
    struct thread_state *thread_state;
    const char          *http = "GET /en-US/docs/Web/HTTP/Messages HTTP/1.1\r\n";
    //                                "Host: example.com\r\n"
    //                                "Content-Type: application/x-www-form-urlencoded\r\n"
    //                                "Content-Length: 50\r\n";

    thread_state = (struct thread_state *)malloc(sizeof(struct thread_state));
    if(thread_state == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    thread_state->request_line_string = (char *)malloc(strlen(http) + 1);
    if(thread_state->request_line_string == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    thread_state->request_line_string_len = strlen(http);
    strlcpy(thread_state->request_line_string, http, thread_state->request_line_string_len);
    parse_request_line(thread_state);
    free(thread_state->request_line_string);

    return 0;
}
