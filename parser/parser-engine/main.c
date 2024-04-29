#include <string.h>
#include "parser.h"

char** init_args(size_t, const char**);

int main(int argc, const char** argv) {
    char** args = init_args(argc + 2, argv);
    if (args == NULL) {
        exit(1);
    }
    int query_status = query(args);
    if (query_status < 0) {
        fprintf(stderr, "error: query failed, abort\n");
        exit(1);
    }

    return 0;
}

char** init_args(size_t args_size, const char** argv) {
    char** args = malloc(sizeof(char*) * args_size);
    if (args == NULL) {
        perror("malloc");
        return NULL;
    }
    /**
     * Put QUERY_DRIVER and RESPONSE_LOC as the first two entries into
     * *args*, with the rest, except for the last entry, which is NULL,
     * being the command-line arguments.
     * **/
    args[0] = malloc(sizeof(char) * (strlen(QUERY_DRIVER) + 1));
    if (args[0] == NULL) {
        perror("malloc");
        free(args), args = NULL;
        return NULL;
    }
    strncpy(args[0], QUERY_DRIVER, strlen(QUERY_DRIVER));
    args[0][strlen(QUERY_DRIVER)] = '\0';
    args[1] = malloc(sizeof(char) * (strlen(RESPONSE_LOC) + 1));
    if (args[1] == NULL) {
        perror("malloc");
        free(args[0]), args[0] = NULL;
        free(args), args = NULL;
        return NULL;
    }
    strncpy(args[1], RESPONSE_LOC, strlen(RESPONSE_LOC));
    args[1][strlen(RESPONSE_LOC)] = '\0';
    args[args_size - 1] = NULL;
    for (int i = 2; i < args_size - 1; i++) {
        args[i] = malloc(sizeof(char) * (strlen(argv[i - 1]) + 1));
        if (args[i] == NULL) {
            perror("malloc");
            for (int j = 0; j < i; j++) {
                free(args[i]), args[i] = NULL;
            }
            free(args), args = NULL;
            return NULL;
        }
        strncpy(args[i], argv[i - 1], strlen(argv[i - 1]));
        args[i][strlen(argv[i - 1])] = '\0';
    }
    return args;
}
