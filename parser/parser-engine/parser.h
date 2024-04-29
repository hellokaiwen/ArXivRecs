#ifndef XML_PARSER_PARSER_H
#define XML_PARSER_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define QUERY_DRIVER "../parser-util/driver.sh"
#define RESPONSE_LOC "../parser-tmp/response.xml"
#define NAMESPACE "ns="http://www.w3.org/2005/Atom""

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

int query(char**);
struct paper_t* parse(const char*);

#endif
