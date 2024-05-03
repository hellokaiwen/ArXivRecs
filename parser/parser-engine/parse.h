#ifndef XML_PARSER_PARSER_H
#define XML_PARSER_PARSER_H

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <curl/curl.h>

#define RESPONSE_LOC "../parser-tmp/response.xml"

struct author_t {
    char* name;
};

struct paper_t {
    char* id;
    char* title;
    char* summary;
    char* category;
    struct author_t** authors;
    char* jour_ref;
};

int fetch_response(const char*, const char**, const size_t*);

struct paper_t* parse(const char*);

#endif
