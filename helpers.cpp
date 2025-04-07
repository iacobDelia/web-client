#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"
#include "constants.h"
#include "json.hpp"
#include "requests.h"
#include <iostream>
#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)
using namespace std;

char* sendRegisterLogin(string username, string password, string type){
    // make the object and turn it into string
    nlohmann::json obj{{"username", username}, {"password", password}};
    string objString = obj.dump(4);

    int sock = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    char *message;
    if(type == "REGISTERPLZ")
        message = compute_post_request(IP, URLREGISTER, TYPE, objString, "", "");
    else message = compute_post_request(IP, URLLOGIN, TYPE, objString, "", "");

    send_to_server(sock, message);
    free(message);
    char* resp = receive_from_server(sock);

    return resp;
}
char* sendLogOut(string cookies){
    int sock = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    char *message = compute_get_request(IP, URLLOGOUT, nullptr, cookies, "");
    send_to_server(sock, message);
    free(message);
    char *resp = receive_from_server(sock);
    return resp;
}
char* sendEnterLibrary(string cookies){
    int sock = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    char *message = compute_get_request(IP, URLACCESS, nullptr, cookies, "");
    send_to_server(sock, message);
    free(message);
    char *resp = receive_from_server(sock);
    return resp;
}
char* sendGetBooks(string jwt){
    int sock = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    char *message = compute_get_request(IP, URLBOOKS, nullptr, "", jwt);
    send_to_server(sock, message);
    free(message);
    char *resp = receive_from_server(sock);
    return resp;
}
char* sendAddBook(string jwt, string title, string author, string genre, string publisher, int pageCount){
    int sock = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    nlohmann::json book{{"title", title}, {"author", author}, {"genre", genre},
                        {"author", author}, {"publisher", publisher},
                        {"page_count", pageCount}};
    string bookString = book.dump(4);
    
    char *message = compute_post_request(IP, URLBOOKS, TYPE, bookString, "", jwt);
    send_to_server(sock, message);
    free(message);
    char *resp = receive_from_server(sock);
    return resp;
}
char* sendGetBookByID(string jwt, string url){
    int sock = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    char *message = compute_get_request(IP, url.c_str(), nullptr, "", jwt);
    send_to_server(sock, message);
    free(message);
    char *resp = receive_from_server(sock);
    return resp;
}
char* sendDeleteReq(string jwt, string url){
    int sock = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    char *message = compute_delete_request(IP, url.c_str(), nullptr, "", jwt);
    send_to_server(sock, message);
    free(message);
    char *resp = receive_from_server(sock);
    return resp;
}
string getThatCookie(string msg){
    size_t cookiePointer = msg.find("Set-Cookie: ");
    if(cookiePointer != msg.npos){
        cookiePointer = cookiePointer + strlen("Set-Cookie: ");
        size_t cookieEnd = msg.substr(cookiePointer).find(";");
        return msg.substr(cookiePointer, cookieEnd);
    }
    return "";
}
string getThatJWT(char* msg){
    char* jsonString = basic_extract_json_response(msg);
    nlohmann::json myJson = nlohmann::json::parse(jsonString);
    string token = myJson["token"].get<string>();
    return token;
}
bool isNumber(string s){
    for(auto chr : s)
        if(!isdigit(chr))
            return false;
    return true;
}
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(const char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}
