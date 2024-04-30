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
#define NAMESPACE "ns=\"http://www.w3.org/2005/Atom\""

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


char* build_query(size_t, const char*);

int fetch_response(const size_t*, const char**);

struct paper_t* parse(const char*);

#endif
