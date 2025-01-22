//
// Created by blaise-klein on 1/9/25.
//

#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <p101_fsm/fsm.h>

in_port_t        parse_in_port_t(const char *port_str, int *err);
void             convert_address(const char *address, struct sockaddr_storage *addr, socklen_t *addr_len, int *err);
int              socket_create(int domain, int type, int protocol, int *err);
void             socket_bind(int sockfd, struct sockaddr_storage *addr, in_port_t port, int *err);
void             socket_close(int sockfd);
void             get_address_to_server(struct sockaddr_storage *addr, in_port_t port, int *err);
ssize_t          read_fully(int sockfd, void *buf, ssize_t len, int *err);
ssize_t          write_fully(int sockfd, const void *buf, ssize_t len, int *err);
p101_fsm_state_t setup_socket(const struct p101_env *env, struct p101_error *err, void *arg);
p101_fsm_state_t await_client_connection(const struct p101_env *env, struct p101_error *err, void *arg);
p101_fsm_state_t start_client_thread(const struct p101_env *env, struct p101_error *err, void *arg);

#endif    // NETWORK_H
