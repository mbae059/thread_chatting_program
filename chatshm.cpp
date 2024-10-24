#include "chatshm.h"
#include <cstring>
#include <string>
#include <algorithm>

void ChatInfo::copy(ChatInfo chatInfo) {
    memcpy(userID, chatInfo.userID, sizeof(userID));
    memcpy(message, chatInfo.message, sizeof(message));
}

std::string ChatInfo::getMessage() {
    std::string ret;
    for(int i=0;i<40;i++) {
        if(message[i]==0) break;
        ret.push_back(message[i]);
    }
    return ret;
}
void ChatLog::push(ChatInfo chatInfo) {
    if(counter<CHAT_LOG_SIZE) {
        chatInfos[counter].copy(chatInfo);
        counter++;
    }
    else {
        for(int i=0;i<CHAT_LOG_SIZE-1;i++) {
            chatInfos[i].copy(chatInfos[i+1]);
        }
        chatInfos[CHAT_LOG_SIZE-1].copy(chatInfo);
    }
}

ChatInfo::ChatInfo() {
    for(int i=0;i<20;i++) userID[i] = 0;
    for(int i=0;i<40;i++) message[i] = 0;
}

ChatInfo::ChatInfo(char _userID[20], char _message[40]) {
    memcpy(userID, _userID, sizeof(userID));
    memcpy(message, _message, sizeof(message));
}
User::User() {
    for(int i=0;i<20;i++) userID[i] = 0;
}

User& User::operator=(char* username) {
    int n = strlen(username);
    for(int i=0;i<20;i++) userID[i] = 0;
    for(int i=0;i<std::min(20, n);i++) {
        userID[i] = username[i];
    }
    return *this;
}

bool User::operator==(char* username) const {
    return std::strcmp(userID, username)==0;
}

std::string User::getString() {
    std::string ret;
    for(int i=0;i<strlen(userID);i++) {
        ret.push_back(userID[i]);
    }
    return ret;
}

void Users::push(char username[20]) {
    if(counter<3) {
        for(int i=0;i<20;i++) {
            users[counter].userID[i] = username[i];
        }
        counter++;
    }
}

void Users::pop(char* username) {
    for(int i=0;i<counter;i++) { //maximum of 3
        if(strcmp(users[i].userID,username)==0) {
            for(int j=i;j<counter-1;j++) {
                users[j] = users[j+1];
            }
            counter--;
            return;
        }
    }
}