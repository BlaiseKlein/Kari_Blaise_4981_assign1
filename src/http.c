//
// Created by blaise-klein on 1/9/25.
//

#include "http.h"

void *parse_request(void *context_data)
{
    int                  err    = 0;
    struct thread_state *data   = (struct thread_state *)context_data;
    ssize_t              result = (ssize_t)read_until(data, "\r\n\r\n", &err);
    if(result < 0)
    {
        return NULL;
    }

    return data;
}

size_t read_until(struct thread_state *data, const char *delimiter, int *err)
{
    ssize_t buffer_end = 0;
    char   *message    = (char *)malloc(MAXMESSAGELENGTH);
    if(message == NULL)
    {
        *err = -1;
        return 0;
    }
    data->request_string = (char *)malloc((MAXMESSAGELENGTH) * sizeof(char));
    if(data->request_string == NULL)
    {
        *err = -1;
        free(message);
        return 0;
    }

    while(strstr(message, delimiter) == message)
    {
        ssize_t result;

        result = read(data->client_fd, message + buffer_end, MAXMESSAGELENGTH);
        if(result < 0)
        {
            *err = -1;
            free(message);
            return 0;
        }

        if(buffer_end + result > MAXMESSAGELENGTH)
        {
            *err = -2;
            free(message);
            return 0;
        }
        buffer_end += result;
    }

    memset(data->request_string, 0, MAXMESSAGELENGTH);
    memcpy(data->request_string, message, (size_t)buffer_end);

    free(message);
    return (size_t)buffer_end;
}