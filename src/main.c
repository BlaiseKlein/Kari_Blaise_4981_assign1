#include "data_types.h"
#include "http.h"
#include "network.h"
#include <p101_fsm/fsm.h>
#include <p101_posix/p101_unistd.h>
#include <stdio.h>
#include <stdlib.h>

static void             parse_arguments(const struct p101_env *env, int argc, char *argv[], struct context *ctx);
_Noreturn static void   usage(const char *program_name, int exit_code, const char *message);
static p101_fsm_state_t error_state(const struct p101_env *env, struct p101_error *err, void *arg);

#define UNKNOWN_OPTION_MESSAGE_LEN 24
#define MAXARGS 5

int main(int argc, char *argv[])
{
    struct p101_error    *error;
    struct p101_env      *env;
    struct p101_error    *fsm_error;
    struct p101_env      *fsm_env;
    struct p101_fsm_info *fsm;
    struct context       *ctx;

    error = p101_error_create(false);
    env   = p101_env_create(error, true, NULL);
    ctx   = (struct context *)malloc(sizeof(struct context));

    if(ctx == NULL)
    {
        usage(argv[0], EXIT_FAILURE, "Context malloc failed");
    }

    ctx->network.receive_addr = NULL;
    parse_arguments(env, argc, argv, ctx);
    fsm_error = p101_error_create(false);
    fsm_env   = p101_env_create(error, true, NULL);
    fsm       = p101_fsm_info_create(env, error, "test-fsm", fsm_env, fsm_error, NULL);

    if(p101_error_has_error(error))
    {
        fprintf(stderr, "Error creating FSM: %s\n", p101_error_get_message(error));
    }
    else
    {
        static struct p101_fsm_transition transitions[] = {
            {P101_FSM_INIT, SETUP_SOCKET,  setup_socket           },
            {SETUP_SOCKET,  AWAIT_CLIENT,  await_client_connection},
            {SETUP_SOCKET,  ERROR_STATE,   error_state            },
            {AWAIT_CLIENT,  CLIENT_THREAD, start_client_thread    },
            {AWAIT_CLIENT,  ERROR_STATE,   error_state            },
            {ERROR_STATE,   P101_FSM_EXIT, NULL                   }
        };
        p101_fsm_state_t from_state;
        p101_fsm_state_t to_state;

        p101_fsm_run(fsm, &from_state, &to_state, ctx, transitions, sizeof(transitions));
        p101_fsm_info_destroy(env, &fsm);
    }

    free(ctx);
    free(fsm_env);
    free(env);
    p101_error_reset(error);
    free(error);

    return EXIT_SUCCESS;
}

static void parse_arguments(const struct p101_env *env, int argc, char *argv[], struct context *ctx)
{
    int opt;

    opterr = 0;

    while((opt = p101_getopt(env, argc, argv, "i:p:h")) != -1)
    {
        switch(opt)
        {
            case 'i':
            {
                ctx->arg.sys_addr     = optarg;
                ctx->arg.sys_addr_len = (ssize_t)strlen(optarg);
                break;
            }
            case 'p':
            {
                ctx->arg.sys_port = optarg;
                break;
            }
            case 'h':
            {
                usage(argv[0], EXIT_SUCCESS, NULL);
            }
            case '?':
            {
                if(optopt == 'c')
                {
                    usage(argv[0], EXIT_FAILURE, "Option '-c' requires a value.");
                }
                else
                {
                    char message[UNKNOWN_OPTION_MESSAGE_LEN];

                    snprintf(message, sizeof(message), "Unknown option '-%c'.", optopt);
                    usage(argv[0], EXIT_FAILURE, message);
                }
            }
            default:
            {
                usage(argv[0], EXIT_FAILURE, NULL);
            }
        }
    }

    if(optind < argc)
    {
        usage(argv[0], EXIT_FAILURE, "Too many arguments.");
    }
    if(argc != 2 && argc != MAXARGS)
    {
        usage(argv[0], EXIT_FAILURE, "Wrong number of arguments.");
    }
}

_Noreturn static void usage(const char *program_name, int exit_code, const char *message)
{
    if(message)
    {
        fprintf(stderr, "%s\n", message);
    }

    fprintf(stderr, "Usage: %s -i <server_ip> -p <server_port>\n", program_name);
    fputs("Options:\n", stderr);
    fputs("  -i  <server_ip> The ip flag and server IP address\n", stderr);
    fputs("  -p  <server_port> The port flag and the port id to communicate with\n", stderr);
    exit(exit_code);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static p101_fsm_state_t error_state(const struct p101_env *env, struct p101_error *err, void *arg)
{
    struct context *ctx = (struct context *)arg;

    if(ctx->network.receive_addr != NULL)
    {
        free(ctx->network.receive_addr);
        ctx->network.receive_addr = NULL;
    }
    printf("An error occured");

    return P101_FSM_EXIT;
}

#pragma GCC diagnostic pop
