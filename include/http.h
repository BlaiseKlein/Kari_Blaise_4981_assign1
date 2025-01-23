//
// Created by blaise-klein on 1/9/25.
//

#ifndef HTTP_H
#define HTTP_H
#include "data_types.h"

#define MAXMESSAGELENGTH 4096
#define MAXLINELENGTH 2048

void  *parse_request(void *data);
size_t read_until(int fd, char *buffer, size_t len, const char *delimiter, int *err);
int    parse_request_headers(struct thread_state *data);
int    parse_request_line(struct thread_state *data);
void   cleanup_headers(struct thread_state *data);
void   cleanup_header(char *header);
int    parse_header(struct thread_state *data, char **buffer, bool *breaks, bool *continues);

#endif    // HTTP_H
