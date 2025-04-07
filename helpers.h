#ifndef _HELPERS_
#define _HELPERS_


#include <string>
// extracts the cookie from a message
std::string getThatCookie(std::string msg);

// constructs and sends a json register request
char* sendRegisterLogin(std::string username, std::string password, std::string type);

// sends a logout request
char* sendLogOut(std::string cookies);

// sends a request for access to the library (to get the jwt)
char* sendEnterLibrary(std::string cookies);

// extracts the jwt from a message
std::string getThatJWT(char* msg);

// sends a request to get book
char* sendGetBookByID(std::string jwt, std::string url);

// checks if a string contains only digits
bool isNumber(std::string s);

// sends a request to get all the books
char* sendGetBooks(std::string jwt);

// sends a request to add a book
char* sendAddBook(std::string jwt, std::string title, std::string author, std::string genre, std::string publisher, int pageCount);

// sends a book delete request
char* sendDeleteReq(std::string jwt, std::string url);

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(const char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

#endif
