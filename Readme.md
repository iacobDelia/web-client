# CLIENT WEB
This project aims to implement a web client that communicates with the server using the REST API.

## Program flow
The server starts an infinite loop, in which it listens for any commands:
### register
The client reads the user and password entered by the user, and checks whether or not they contain any spaces. If they do, a message is shown on screen and the user can try to register another account.

If the user chooses a username and password that are valid, the client opens a socket to the server, composes a POST http request containing the json object corresponding to the username and password, and sends it to the server. The response is then interpreted by the client, and an appropriate message is shown.

### login
The client first checks if the cookies contain anything. If they do, that means the user is already logged in, and the client can't allow them to log in again. If they're not, the username and password are read and checked to see if they contain any white spaces.

If the user is not logged in and the credentials are valid, the client composes and sends a POST http request containing the json with the credentials. If the user has registered before, the server will return the cookies corresponding to that session, which the client will store for later.

### enter_library
The client sends a GET http request containing the cookies, and extracts the jwt token given in the response.

### add_book
The book information is read, and checked to be correct. If everything is good, then a json object containing the book info is composed, and sent to the server in a POST request alongside the jwt token. An appropriate response is shown if the operation completed successfully.

### get_book
The book id is read and checked to be correct, and a GET request is sent to the server containing the url and id. The client then extracts and shows the json information about that book, or an error message.

### get_books
Same as get_book, but no id is sent, only the url.

### delete_book
The book id is read and checked to be correct, and a DELETE request is sent to the server, containing the url and id just like in add_book. The response from the server is then interpreted.

### exit
Stops the loop and exits the program.
