#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

#include "helpers.h"
#include "constants.h"
#include <limits>
using namespace std;

string username, password;
void readUserAndPass(){
// read the username and password
    getline(cin, username);
    cout<<"username=";
    getline(cin, username);

    if(username.find(" ") != username.npos){
        username = "INVALID ";
    }
    cin.clear();

    cout<<"password=";
    getline(cin, password);

    if(password.find(" ") != username.npos){
        password = "INVALID ";
    }
    cin.clear();

}
string title, author, genre, publisher, pageCountString;
int pageCount;
// read the details for books
void readBookDetails(){
    getline(cin, title);
    cout<<"title=";
    getline(cin, title);
    cout<<"author=";
    getline(cin, author);
    cout<<"genre=";
    getline(cin, genre);
    cout<<"publisher=";
    getline(cin, publisher);
    cout<<"page_count=";
    getline(cin, pageCountString);

    if(!isNumber(pageCountString)){
        cout<<"ERROR invalid pages number\n";
        pageCountString = "INVALID ";
        cin.clear();
        return;
    }
    cin.clear();
    pageCount = atoi(pageCountString.c_str());
}
bool checkBookDetails(){
    if(title == "" || author == "" ||
    genre == "" || publisher == "" ||
    pageCountString == "") {
        cout<<"ERROR please dont leave any of the book fields empty!\n";
        return false;
    }
    return true;
}
string stringID;
// read the book id
void readBookID(){
    getline(cin, stringID);
    cout<<"id=";
    getline(cin, stringID);
    if(!isNumber(stringID)){
        cout<<"ERROR Invalid input. Please only enter numbers.\n";
        stringID = "INVALID ";
    }
    cin.clear();
}
int main(){
    bool serverIsRunning = true;
    string command, cookies, jwt;
    // start the server
    while(serverIsRunning){
        cin>>command;
        if(command.compare("register") == 0){
            readUserAndPass();
            if(password == "INVALID " || username == "INVALID "){
                cout<<"ERROR Invalid user or password, no whitespaces plz\n";
                continue;
            }
            // send them away and receive a response
            char* resp = sendRegisterLogin(username, password, "REGISTERPLZ");
            
            // interpret the response
            string response  = resp;
            if(response.find("HTTP/1.1 2") != response.npos)
                cout<<"200 - OK SUCCESS creating the account\n";
            else {
                cout<<"ERROR creating the account\n";
                cout<<response<<endl;}

            free(resp);
        } else if(command.compare("login") == 0){
            if(!cookies.empty()){
                cout<<"ERROR: already logged in\n";
                continue;
            } 
            readUserAndPass();
            if(password == "INVALID " || username == "INVALID "){
                cout<<"ERROR Invalid user or password, no whitespaces plz\n";
                continue;
            }
            // send request and receive a response
            char* resp = sendRegisterLogin(username, password, "LOGINPLZ");
            string response  = resp;
            cookies = getThatCookie(response);

            if(response.find("HTTP/1.1 2") != response.npos)
                cout<<"200 - OK SUCCESS logging in yay\n";
            else {
                cout<<"ERROR logging in nay\n";
                cout<<response<<endl;
            }
            
        } else if(command.compare("logout") == 0){
            string response = sendLogOut(cookies);

            if(response.find("HTTP/1.1 2") != response.npos){
                cout<<"200 - OK SUCCESS logging out\n";
                cookies.clear();
                jwt.clear();
            } else{
                 cout<<"ERROR logging out\n";
                 cout<<response<<endl;
            }
        } else if(command.compare("enter_library") == 0){
            char* resp = sendEnterLibrary(cookies);
            string response = resp;

            if(response.find("HTTP/1.1 2") != response.npos){
                jwt = getThatJWT(resp);
                cout<<"200 - OK SUCCESS entering the library\n";
            } else{
                cout<<"ERROR entering the library\n";
                cout<<response<<endl;
            } 
        } else if(command.compare("get_books") == 0){
            char* resp = sendGetBooks(jwt);
            string response = resp;
            // extract the json and show the message
            if(response.find("HTTP/1.1 2") != response.npos){
                char *jsonMsg = basic_extract_json_response(resp);
                if(jsonMsg == 0x0)
                    cout<<"No books here!"<<endl;
                else cout<<jsonMsg<<endl;
            } else {
                cout<<"ERROR getting the books\n";
                cout<<response<<endl;
            }
        } else if(command.compare("add_book") == 0){
            readBookDetails();
            if(pageCountString == "INVALID " || !checkBookDetails())
                continue;
            
            string response = sendAddBook(jwt, title, author, genre, publisher, pageCount);

            if(response.find("HTTP/1.1 2") != response.npos){
                cout<<"200 - OK SUCCESS adding the book to the library\n";
            } else{
                cout<<"ERROR adding the book\n";
                cout<<response<<endl;
            } 
        } else if(command.compare("get_book") == 0){
            readBookID();
            if(stringID == "INVALID "){
                continue;
            }
            string url = "/api/v1/tema/library/books/" + stringID;
            char* resp = sendGetBookByID(jwt, url);
            string response = resp;

            if(response.find("HTTP/1.1 2") != response.npos){
                char *jsonMsg = basic_extract_json_response(resp);
                if(jsonMsg == 0x0)
                    cout<<"No books here!"<<endl;
                else cout<<jsonMsg<<endl;
            } else{
                cout<<"ERROR getting book info\n";
                cout<<response<<endl;
            }
        } else if(command.compare("delete_book") == 0){
            readBookID();
            // if the id isn't a number
            if(stringID == "INVALID ")
                continue;
            
            string url = "/api/v1/tema/library/books/" + stringID;
            string response = sendDeleteReq(jwt, url);

            if(response.find("HTTP/1.1 2") != response.npos){
                cout<<"200 - OK SUCCESS deleting the book\n";
            } else{
                cout<<"ERROR deleting the book\n";
                cout<<response<<endl;
            } 
        } else if(command.compare("exit") == 0){
            serverIsRunning = false;
        } else cout<<"Unknown command!\n";
    } 
    return 0;
}