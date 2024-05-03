#include <string.h>
#include "parse.h"

char** init_args(size_t, const char**);

int main(int argc, const char** argv) {
    if (argc < 3) {
        fprintf(stderr, "error: invalid number of arguments, abort\n");
        exit(1);
    }
    char** args = init_args(argc, argv);
    if (args == NULL) {
        exit(1);
    }
    size_t starts[argc];
    for (size_t i = 1; i < argc; i++) {
        starts[i] = 0;
    }
    int query_status = fetch_response(argv[1], (const char**) args, (const size_t*) starts);
    if (query_status == 1) {
        fprintf(stderr, "error: query failed, abort\n");
        exit(1);
    }

    // TODO: parse the response
    return 0;
}

char** init_args(size_t args_size, const char** argv) {
    char** args = malloc(sizeof(char*) * args_size);
    if (args == NULL) {
        perror("malloc");
        return NULL;
    }
    args[0] = malloc(sizeof(char) * (strlen(RESPONSE_LOC) + 1));
    if (args[0] == NULL) {
        perror("malloc");
        free(args), args = NULL;
        return NULL;
    }
    strncpy(args[0], RESPONSE_LOC, strlen(RESPONSE_LOC));
    args[0][strlen(RESPONSE_LOC)] = '\0';
    args[args_size - 1] = NULL;
    for (size_t i = 1; i < args_size - 1; i++) {
        args[i] = malloc(sizeof(char) * (strlen(argv[i + 1]) + 1));
        if (args[i] == NULL) {
            perror("malloc");
            for (size_t j = 0; j < i; j++) {
                free(args[j]), args[j] = NULL;
            }
            free(args), args = NULL;
            return NULL;
        }
        strncpy(args[i], argv[i + 1], strlen(argv[i + 1]));
        args[i][strlen(argv[i + 1])] = '\0';
    }
    return args;
}
