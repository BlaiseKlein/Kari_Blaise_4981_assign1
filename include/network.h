//
// Created by blaise-klein on 1/9/25.
//

#ifndef NETWORK_H
#define NETWORK_H
#include "data_types.h"
#include "http.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

in_port_t               parse_in_port_t(const char *port_str, int *err);
void                    convert_address(const char *address, struct sockaddr_storage *addr, socklen_t *addr_len, int *err);
int                     socket_create(int domain, int type, int protocol, int *err);
void                    socket_bind(int sockfd, struct sockaddr_storage *addr, in_port_t port, int *err);
void                    socket_close(int sockfd);
void                    get_address_to_server(struct sockaddr_storage *addr, in_port_t port, int *err);
ssize_t                 read_fully(int sockfd, void *buf, ssize_t len, int *err);
ssize_t                 write_fully(int sockfd, const void *buf, ssize_t len, int *err);
static p101_fsm_state_t setup_socket(const struct p101_env *env, struct p101_error *err, void *arg);
static p101_fsm_state_t await_client_connection(const struct p101_env *env, struct p101_error *err, void *arg);
static p101_fsm_state_t start_client_thread(const struct p101_env *env, struct p101_error *err, void *arg);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"

static p101_fsm_state_t await_client_connection(const struct p101_env *env, struct p101_error *err, void *arg)
{
    struct context *ctx = (struct context *)arg;
    printf("Waiting for connection\n");

    while(1)
    {
        int result;

        result = accept(ctx->network.receive_fd, (struct sockaddr *)ctx->network.receive_addr, &ctx->network.receive_addr_len);
        if(result != 0)
        {
            printf("%d", result);
            printf("%d", errno);
            break;
        }

        return CLIENT_THREAD;
    }

    return ERROR_STATE;
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"

static p101_fsm_state_t start_client_thread(const struct p101_env *env, struct p101_error *err, void *arg)
{
    // pthread_t       id;
    struct context *ctx = (struct context *)arg;
    // int             thread_creation_result = 0;

    struct thread_state *client_state = (struct thread_state *)malloc(sizeof(struct thread_state));
    printf("Connection obtained\n");
    if(client_state == NULL)
    {
        return ERROR_STATE;
    }
    client_state->client_fd = ctx->network.next_client_fd;

    // thread_creation_result = pthread_create(&id, NULL, parse_request, client_state);
    // if(thread_creation_result != 0)
    // {
    //     return ERROR_STATE;
    // }
    parse_request(client_state);

    return AWAIT_CLIENT;
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"

static p101_fsm_state_t setup_socket(const struct p101_env *env, struct p101_error *err, void *arg)
{
    struct context *ctx       = (struct context *)arg;
    ctx->network.receive_addr = (struct sockaddr_storage *)malloc(sizeof(struct sockaddr_storage));

    ctx->network.receive_port = parse_in_port_t(ctx->arg.sys_port, &ctx->err);
    if(ctx->err < 0)
    {
        return ERROR_STATE;
    }
    convert_address(ctx->arg.sys_addr, ctx->network.receive_addr, &ctx->network.receive_addr_len, &ctx->err);
    if(ctx->err < 0)
    {
        return ERROR_STATE;
    }
    ctx->network.receive_fd = socket_create(ctx->network.receive_addr->ss_family, SOCK_DGRAM, 0, &ctx->err);
    if(ctx->err < 0)
    {
        return ERROR_STATE;
    }
    socket_bind(ctx->network.receive_fd, ctx->network.receive_addr, ctx->network.receive_port, &ctx->err);
    if(ctx->err < 0)
    {
        return ERROR_STATE;
    }

    ctx->network.msg_size = sizeof(uint16_t);

    return AWAIT_CLIENT;
}

#pragma GCC diagnostic pop

#endif    // NETWORK_H
