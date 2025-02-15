//
// Created by blaise-klein on 1/9/25.
//

#ifndef DATA_TYPES_H
#define DATA_TYPES_H
#include <netinet/in.h>
#include <p101_fsm/fsm.h>
#include <stdint.h>
#include <sys/types.h>

enum http_method
{
    GET,
    HEAD
};

struct arguments
{
    char   *sys_addr;
    ssize_t sys_addr_len;
    char   *sys_port;
};

struct network_state
{
    int                      next_client_fd;
    int                      receive_fd;
    struct sockaddr_storage *receive_addr;
    socklen_t                receive_addr_len;
    in_port_t                receive_port;
};

struct thread_state
{
    int              client_fd;
    char            *request_line_string;
    size_t           request_line_string_len;
    char            *date_header;
    char            *pragma_header;
    char            *auth_header;
    char            *from_header;
    char            *if_modified_since_header;
    char            *referer_header;
    char            *user_agent_header;
    char            *allow_header;
    char            *content_encoding_header;
    char            *content_length_header;
    char            *content_type_header;
    char            *expires_header;
    char            *last_modified_header;
    char            *resource_string;
    enum http_method method;
    int              err;
    char            *version;
};

struct context
{
    struct arguments     arg;
    struct network_state network;
    int                  err;
};

enum application_states
{
    SETUP_SOCKET = P101_FSM_USER_START,    // 2
    AWAIT_CLIENT,
    CLIENT_THREAD,
    ERROR_STATE,
};
#endif    // DATA_TYPES_H
