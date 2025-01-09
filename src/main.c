#include <p101_fsm/fsm.h>
#include <p101_posix/p101_unistd.h>
#include <stdio.h>
#include <stdlib.h>

static void             parse_arguments(const struct p101_env *env, int argc, char *argv[]);
_Noreturn static void   usage(const char *program_name, int exit_code, const char *message);
static p101_fsm_state_t a(const struct p101_env *env, struct p101_error *err, void *arg);
static p101_fsm_state_t setup_socket(const struct p101_env *env, struct p101_error *err, void *arg);
static p101_fsm_state_t await_client_connection(const struct p101_env *env, struct p101_error *err, void *arg);
static p101_fsm_state_t start_client_thread(const struct p101_env *env, struct p101_error *err, void *arg);
static p101_fsm_state_t error_state(const struct p101_env *env, struct p101_error *err, void *arg);
static void             will_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id);
static void             did_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id, p101_fsm_state_t next_state_id);
static p101_fsm_state_t bad_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id);

struct context
{
    struct arguments     arg;
    struct network_state network;
    int                  err;
    int                  input_rdy;
    int                  net_rdy;
};

enum application_states
{
    SETUP_SOCKET = P101_FSM_USER_START,    // 2
    AWAIT_CLIENT,
    CLIENT_THREAD,
    ERROR_STATE,
};

#define UNKNOWN_OPTION_MESSAGE_LEN 24

int main(int argc, char *argv[])
{
    struct p101_error    *error;
    struct p101_env      *env;
    bool                  bad;
    bool                  will;
    bool                  did;
    struct p101_error    *fsm_error;
    struct p101_env      *fsm_env;
    struct p101_fsm_info *fsm;

    error = p101_error_create(false);
    env   = p101_env_create(error, true, NULL);
    parse_arguments(env, argc, argv);
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
            {P101_FSM_INIT, SETUP_SOCKET,             setup_socket          },
            {SETUP_SOCKET,             AWAIT_CLIENT,             await_client_connection          },
            {SETUP_SOCKET,             ERROR_STATE,             error_state          },
            {AWAIT_CLIENT,             CLIENT_THREAD,             start_client_thread          },
            {AWAIT_CLIENT,             ERROR_STATE,         error_state},
            {ERROR_STATE,         P101_FSM_EXIT, NULL       }
        };
        p101_fsm_state_t from_state;
        p101_fsm_state_t to_state;
        int              count;

        count = 0;
        p101_fsm_run(fsm, &from_state, &to_state, &count, transitions, sizeof(transitions));
        p101_fsm_info_destroy(env, &fsm);
    }

    free(fsm_env);
    free(env);
    p101_error_reset(error);
    free(error);

    return EXIT_SUCCESS;
}

static void parse_arguments(const struct p101_env *env, int argc, char *argv[], )
{
    int opt;

    opterr = 0;

    while((opt = p101_getopt(env, argc, argv, "i:p:")) != -1)
    {
        switch(opt)
        {
            case 'i':
            {
                break;
            }
            case 'p':
            {
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
}

_Noreturn static void usage(const char *program_name, int exit_code, const char *message)
{
    if(message)
    {
        fprintf(stderr, "%s\n", message);
    }

    fprintf(stderr, "Usage: %s [-h] [-b] [-d] [-w]\n", program_name);
    fputs("Options:\n", stderr);
    fputs("  -h   Display this help message\n", stderr);
    fputs("  -b   Display 'bad' transitions\n", stderr);
    fputs("  -w   Display 'will' transitions\n", stderr);
    fputs("  -d   Display 'did' transitions\n", stderr);
    exit(exit_code);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static p101_fsm_state_t a(const struct p101_env *env, struct p101_error *err, void *arg)
{
    int *count;

    P101_TRACE(env);
    count = ((int *)arg);
    printf("a called with %d\n", *count);
    *count += 1;

    return B;
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static p101_fsm_state_t state_error(const struct p101_env *env, struct p101_error *err, void *arg)
{
    P101_TRACE(env);

    return P101_FSM_EXIT;
}

#pragma GCC diagnostic pop

static void will_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id)
{
    P101_TRACE(env);
    printf("%s will change from %d to %d\n", p101_fsm_info_get_name(env, info), from_state_id, to_state_id);
}

static void did_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id, p101_fsm_state_t next_state_id)
{
    P101_TRACE(env);
    printf("%s did change from %d to %d\n", p101_fsm_info_get_name(env, info), from_state_id, to_state_id);
}

static p101_fsm_state_t bad_change_state_notifier_func(const struct p101_env *env, struct p101_error *err, const struct p101_fsm_info *info, p101_fsm_state_t from_state_id, p101_fsm_state_t to_state_id)
{
    P101_TRACE(env);
    printf("%s can't change from %d to %d\n", p101_fsm_info_get_name(env, info), from_state_id, to_state_id);

    return to_state_id;
}