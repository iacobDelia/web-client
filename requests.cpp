#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string>

#include "helpers.h"
#include "requests.h"
#include "constants.h"
using namespace std;
char *compute_get_request(const char *host, const char *url, char *query_params,
                            const string& cookies, const string& jwt)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    memset(line, 0, LINELEN);
    if(!jwt.empty()){
        strcpy(line, "Authorization: Bearer ");
        strcat(line, jwt.c_str());
        compute_message(message, line);
    }
    memset(line, 0, LINELEN);
    if (!cookies.empty()) {
        strcpy(line, "Cookie: ");
        strcat(line, cookies.c_str());
        compute_message(message, line);
        }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(const char *host, const char *url, const char* content_type, string &body_data,
                            const string& cookies, const string& jwt)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
   memset(line, 0, LINELEN);
    if(!body_data.empty()){
        sprintf(line, "Content-Type: %s", content_type);
        compute_message(message, line);

        memset(line, 0, LINELEN);
        long len = body_data.length();
        sprintf(line, "Content-Length: %ld", len);
        compute_message(message, line);

        strcat(body_data_buffer, body_data.c_str());
    }

    memset(line, 0, LINELEN);
    if(!jwt.empty()){
        strcpy(line, "Authorization: Bearer ");
        strcat(line, jwt.c_str());
        compute_message(message, line);
    }

    // Step 4 (optional): add cookies
    memset(line, 0, LINELEN);
    if(!cookies.empty()){
        strcpy(line, "Cookie: ");
        strcat(line, cookies.c_str());
        compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");
    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    strcat(message, body_data_buffer);

    free(line);
    free(body_data_buffer);
    return message;
}
char *compute_delete_request(const char *host, const char *url, char *query_params,
                            const string &cookies, const string &jwt)
{
    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *) calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    memset(line, 0, LINELEN);
    if (!jwt.empty()) {
        strcpy(line, "Authorization: Bearer ");
        strcat(line, jwt.c_str());
        compute_message(message, line);
    }

    memset(line, 0, LINELEN);
    if (!cookies.empty()) {
        strcpy(line, "Cookie: ");
        strcat(line, cookies.c_str());
        compute_message(message, line);
    }

    // Step 4: add final new line
    compute_message(message, "");

    free(line);
    return message;
}