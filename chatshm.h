#include <string>

//information about the chatting log.
//used to save in shared_memory
//its purpose is to make chatting data into fixed chunks of memory
#define CHAT_LOG_SIZE 14

/*
There will be two main shared memory part
1. chatting log
2. user id
*/
const int shm_chat_key = 54325234;
const int shm_user_key = 87867532;
struct ChatInfo {
public:
    char userID[20];
    char message[40];

    ChatInfo();
    ChatInfo(char _userID[20], char _message[40]);
    //to copy the data with convenience
    void copy(ChatInfo chatInfo);
    std::string getMessage();
};

/*
Need about 14 lines of message for the reader to make the program look more appealing
10 lines for the output console in writing. But as the input console does not exist in
csechatread program, make the output console a bit more longer
*/

//Stored in shared memory
struct ChatLog {
public:
    ChatInfo chatInfos[CHAT_LOG_SIZE];
    //push makes the data to pop the first data and push the last data at the end of the array
    void push(ChatInfo chatInfo);
    //counter is to keep track of data size
    int counter=0;
};


struct User {
public:
    //user ID
    char userID[20];
    //overriding = and == to make the code more simpler
    User& operator = (char username[20]);
    bool operator == (char username[20]) const;

    //use constructor to initialize userID
    User();
    //make userID to string to utilize
    std::string getString();
    //just return userID
    char* getUserID();
};

//Stored in shared memory
struct Users {
public:
    //there must be less than or equal to 3 users
    User users[3];
    //push user
    //increment username
    void push(char* username);
    //find the user that exits and delete it
    //decrement counter
    void pop(char* username);
    //counter to keep track of how many people are in this chatting program
    int counter=0;
};


