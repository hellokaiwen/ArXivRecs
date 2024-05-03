#include <libxml2/libxml/parser.h>
#include "parse.h"

/**
 * Assume START and MAX are always preceded by a type of search query
 * (e.g., cat, au, etc.).
 **/
#define DEFAULT_URL "http://export.arxiv.org/api/query?search_query="
#define START "&start="
#define MAX "&max_results="

#define MAX_RESULTS 10
#define MAX_LINE_SIZE 256

struct _xml_t {
    uint8_t* content;
    size_t nmemb;
};

FILE* _fopen(const char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        perror("fopen");
        return NULL;
    }
    fprintf(file, "%s", "");
    if (fclose(file) == EOF) {
        perror("fclose");
        return NULL;
    }
    file = fopen(path, "a");
    if (file == NULL) {
        perror("fopen");
        return NULL;
    }
    return file;
}

size_t _int2str_len(size_t n) {
    return (n <= 9) ?
            1 : 1 + _int2str_len(n / 10);
}

char* _int2str(size_t n) {
    size_t len = _int2str_len(n);
    char* strized = malloc(sizeof(char) * (len + 1));
    if (strized == NULL) {
        perror("malloc");
        return NULL;
    }
    for (int i = len - 1; i >= 0; i--) {
        strized[i] = (char) (n % 10 + '0');
        n /= 10;
    }
    return strized;
}

size_t _write_callback(void* ptr, size_t size, size_t nmemb, void* data) {
    struct _xml_t* xml = (struct _xml_t *)data;
    size_t total_size = size * nmemb;
    size_t new_nmemb = xml->nmemb + total_size;
    uint8_t old_content[xml->nmemb];
    memcpy(old_content, xml->content, xml->nmemb);
    uint8_t* old_ptr = xml->content;
    xml->content = malloc(sizeof(uint8_t) * new_nmemb);
    if (xml->content == NULL) {
        perror("malloc");
        xml->content = old_ptr;
        exit(1);
    }
    free(old_ptr), old_ptr = NULL;
    memcpy(xml->content, old_content, xml->nmemb);
    memcpy(xml->content + xml->nmemb, ptr, total_size);
    xml->nmemb = new_nmemb;
    return total_size;
}

char* _build_query(const char* opt, const char* searched, size_t start) {
    char* start_str = _int2str(start);
    char* max_str = _int2str(MAX_RESULTS);
	size_t length = strlen(DEFAULT_URL) + strlen(opt) + strlen(":") + strlen(searched) +
                    strlen(START) + strlen(start_str) + strlen(MAX) + strlen(max_str);
	char* query = malloc(sizeof(char) * (length + 1));
	if (query == NULL) {
		perror("malloc");
		return NULL;
	}
	strncpy(query, DEFAULT_URL, strlen(DEFAULT_URL));
	query[strlen(DEFAULT_URL)] = '\0';
	strncat(query, opt, strlen(opt));
    strncat(query, ":", strlen(":"));
	strncat(query, searched, strlen(searched));
    strncat(query, START, strlen(START));
    strncat(query, start_str, strlen(start_str));
    strncat(query, MAX, strlen(MAX));
    strncat(query, max_str, strlen(max_str));
	query[length] = '\0';
	return query;
}

int fetch_response(const char* search_option, const char** args, const size_t* starts) {
    size_t num_cats = 0;
    for (size_t i = 1; args[i]; i++) { num_cats++; }
	int fds[num_cats + 1][2];
	for (size_t i = 1; i <= num_cats; i++) {
		if (pipe(fds[i]) < 0) {
			perror("pipe");
			return 1;
		}
		pid_t pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(1);
		} else if (pid == 0) {
			for (size_t j = 1; j <= i; j++) {
				close(fds[j][0]);
			}
			char* request = _build_query(search_option, args[i], starts[i]);
			if (request == NULL) {
				close(fds[i][1]);
				exit(1);
			}
            CURL* curl = curl_easy_init();
            if (!curl) {
                fprintf(stderr, "error: failed to initialize CURL object\n");
                close(fds[i][1]);
                exit(1);
            }
            struct _xml_t data = {NULL, 0};
            CURLcode res;
            curl_easy_setopt(curl, CURLOPT_URL, request);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "error: API request failed\n");
                close(fds[i][1]);
                free(request), request = NULL;
                exit(1);
            }
            if (data.nmemb == 0) {
                close(fds[i][1]);
                free(request), request = NULL;
                exit(1);
            }
            ssize_t written;
            written = write(fds[i][1], &data.nmemb, sizeof(size_t));
            if (written == -1) {
                perror("write");
                close(fds[i][1]);
                free(request), request = NULL;
                exit(1);
            }
            written = write(fds[i][1], data.content, sizeof(uint8_t) * data.nmemb);
            if (written == -1) {
                perror("write");
                close(fds[i][1]);
                free(request), request = NULL;
                free(data.content), data.content = NULL;
                exit(1);
            }
            free(data.content), data.content = NULL;
            free(request), request = NULL;
            close(fds[i][1]);
            exit(0);
		} else {
			close(fds[i][1]);
		}
	}
    int exit_code = 0;
    FILE* dest = _fopen(args[0]);
    if (dest == NULL) {
        exit_code = 1;
        goto cleanup;
    }
    for (size_t i = 1; i <= num_cats; i++) {
        int status;
        if (wait(&status) < 0) {
            perror("wait");
            exit_code = 1;
            goto cleanup;
        }
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 1) {
                exit_code = 1;
                goto cleanup;
            }
            size_t nmemb;
            uint8_t* byte_content;
            ssize_t num_read;
            num_read = read(fds[i][0], &nmemb, sizeof(size_t));
            if (num_read < 0) {
                perror("read");
                exit_code = 1;
                goto cleanup;
            }
            byte_content = malloc(sizeof(uint8_t) * nmemb);
            if (byte_content == NULL) {
                perror("malloc");
                exit_code = 1;
                goto cleanup;
            }
            num_read = read(fds[i][0], byte_content, sizeof(uint8_t) * nmemb);
            if (num_read < 0) {
                perror("read");
                close(fds[i][0]);
                return 1;
            }
            char content_buf[nmemb + 1];
            memcpy(content_buf, (char *)byte_content, nmemb);
            content_buf[nmemb] = '\0';
            fprintf(dest, "%s\n", content_buf);
        }
    }
    if (fclose(dest) == EOF) {
        perror("fclose");
        return 1;
    }
cleanup:
    if (exit_code == 1) {
        for (size_t i = 1; i <= num_cats; i++) {
            close(fds[i][0]);
        }
    }
    return exit_code;
}

struct paper_t* parse(const char* path) {
//    xmlDoc* document;
//    xmlNode* root;
//    document = xmlReadFile(path, NULL, 0);
//    root = xmlDocGetRootElement(document);
//    printf("root->name : %s\n", root->name);
    return NULL;
}