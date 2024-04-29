#include "parser.h"

/**
 * Send a query to the ArXiv API to request papers with categories
 * specified by *args*. The first parameter of *args* stores the
 * relative path of the bash script used to send request to the API,
 * and the rest of *args* are the preferred categories collected from
 * the user, except that the last entry of *args* stores NULL that is
 * use to signal the end of *args*.
 * **/
int query(char** args) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        execvp(QUERY_DRIVER, args);
        perror("execvp");
        exit(1);
    } else {
        int status;
        if (wait(&status)) {
            if (WIFEXITED(status) == 1) {
                exit(1);
            } else {
                exit(0);
            }
        }
        exit(1);
    }
}

/**

 * **/
struct paper_t* parse(const char* src) {
    return NULL;
}
