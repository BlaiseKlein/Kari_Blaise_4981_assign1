//
// Created by blaise-klein on 1/9/25.
//

#ifndef HTTP_H
#define HTTP_H
#include "data_types.h"

int parse_request(struct thread_state data);
int read_until(int fd, char* delimiter);

#endif //HTTP_H
