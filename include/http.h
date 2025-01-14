//
// Created by blaise-klein on 1/9/25.
//

#ifndef HTTP_H
#define HTTP_H
#include "data_types.h"
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#define MAXMESSAGELENGTH 4096
#define MAXLINELENGTH 2048

void  *parse_request(void *data);
size_t read_until(struct thread_state *data, const char *delimiter, int *err);
void parse_headers(struct thread_state *data);

#endif    // HTTP_H
