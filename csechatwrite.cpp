#include <ncurses.h>
#include <sys/types.h> // 순서
#include <sys/ipc.h> // types -> shm
#include <sys/shm.h>
#include <string>
#include <deque>
#include <cstring>
#include "chatshm.h"
const int output_column = 40;
const int output_row = 10;

/*
Basic setting for ncurses
set cursor to visible and echo
Check if the argc is appropriate && if could ge shared memory segment && if could attach to that shared memory
*/
void initial_setup(int, char* argv[]);

/*
set the basic setting for each windows.
Also have the box surround the window to make it more visible
Refresh the whole thing and each windows as well
Set the cursor to input console so that we can type in.
*/
void set_windows(WINDOW*, WINDOW*, WINDOW*, WINDOW*, std::deque<std::string>&);

/*
Set the output window to messages that we have given as input
*/
void set_output_window(WINDOW* window, std::deque<std::string>&dq);
/*
Set the username window to users who have accessed csechatwrite program.
Has to be implemented using shared memory
*/
void set_username_window(WINDOW* window);
/*
Set the app name
*/
void set_app_name_window(WINDOW* window);

/*
Quite often Users shared memory has to be accessed a lot due to its failure in constructing this code.
As a result we made this as a function to be called simply
Warning! Has to be detached when called from outside.
Maybe could be implemented using smart pointer to remove any human errors
*/
Users* getUsersPtr();
/*
Does the same purpose as getUsersPtr();
*/
ChatLog* getChatLog();
/*
Surround the window by * to make the window more appealing
*/
void setBox(WINDOW*);

/*
When this program ends, this username has to be deleted from user shared memory.
Implement using getUsersPtr();
*/
void eraseUser(char* username);
/*
When failed to create or get to shared memory segment
*/
void failed_memory_segment();

/*
When failed to attach to memory segment
*/
void failed_attach();

/*
Get the input and insert it into dq.
username has to be passed on as ChatInfo has username array.
When getting the input, get the ChatLog shared memory and push
ChatInfo to ChatLog.
*/
bool input(char msg[40], char *username, std::deque<std::string>& dq);


int main(int argc, char* argv[]) {
    //escape conditions
    initial_setup(argc, argv);

    WINDOW *output_window = newwin(output_row+2, output_column+2, 0, 0);
    WINDOW *username_window = newwin(output_row+2, 18, 0, output_column+3);
    WINDOW *input_window = newwin(3, output_column+2, output_row+3, 0);
    WINDOW *app_name_window = newwin(3, 18, output_row+3, output_column+3);

    //The role that msg does is
    //1. save message to shared memory segment
    //2. output the message in our output_window

    //use of malloc
    char* msg = (char*)malloc(sizeof(char)*40);
    std::deque<std::string> messages;
    do {
        set_windows(output_window, username_window, input_window, app_name_window, messages);
    } while(input(msg, argv[1], messages));

    //free the allocated space to ensure that memory leak does not occur
    free(msg);
    eraseUser(argv[1]);
    
    endwin();
    return 0;
}

void failed_memory_segment() {
    fprintf(stderr, "Error: Failed to create or get shared memory segment.");
    getch();
    endwin();
    exit(1);
}

void failed_attach() {
    fprintf(stderr, "Error: Failed to attach to shared memory segment.");
    endwin();
    exit(1);
}


Users* getUsersPtr() {
    int shmid = shmget((key_t)(shm_user_key), sizeof(Users), 0666|IPC_CREAT);
    if(shmid==-1) failed_memory_segment();
    Users* shmptr = (Users*)shmat(shmid, nullptr, 0666);
    if(shmptr==(Users*)-1) failed_attach();
    return shmptr;
}

ChatLog* getChatLog() {
    int shmid = shmget((key_t)(shm_chat_key), sizeof(ChatLog), 0666|IPC_CREAT);
    if(shmid==-1) failed_memory_segment();
    ChatLog* shmptr = (ChatLog*)shmat(shmid, nullptr, 0666);
    if(shmptr==(ChatLog*)-1) failed_attach();
    return shmptr;
}

void eraseUser(char* username) {
    auto shmptr = getUsersPtr();
    shmptr->pop(username);
    shmdt(shmptr);
}

/*
Have to check 3 main issues
1. check if there is an username. If not exit with error
2. check if there are already 3 people. If less than 2 people, then let this one pass
3. check if failed to create or get shared memory segment with messages or usernames

Plus add current username to users when accessed in this chat program
*/
void initial_setup(int argc, char* argv[]) {
    initscr();
    cbreak();
    curs_set(1);
    echo();
    start_color();

    //checking if there is an username or not
    if (argc != 2) {
        erase();
        refresh();
        fprintf(stderr, "Error! [Usage]: ./csechatwrite UserID \n");
        getch();
        endwin();
        exit(0);
    }
    
    auto shmUsersPtr = getUsersPtr();
    
    if(shmUsersPtr->counter==3) {
        fprintf(stderr, "There are currently 3 people participating. Please try again later\n");
        endwin();
        exit(0);
    }

    shmUsersPtr->push(argv[1]);
    shmdt(shmUsersPtr);

    auto shmChatLogPtr = getChatLog();
    shmdt(shmChatLogPtr);
}

void setBox(WINDOW* window) {
    box(window, '*', '*');
}

void set_output_window(WINDOW* window, std::deque<std::string>&dq) {
    for(int i=0;i<dq.size();i++) {
        mvwprintw(window, i+1, 1, "%s\n", dq[i].c_str());
    }
}

void set_username_window(WINDOW* window) {
    auto ptr = getUsersPtr();
    werase(window);

    for(int i=0;i<ptr->counter;i++) {
        std::string str = ptr->users[i].getString();
        mvwprintw(window, i+1, 1, "%s\n", str.c_str());
    }
    shmdt(ptr);
}

void set_app_name_window(WINDOW* window) {
    mvwprintw(window, 1, 1, "Enter \\q to Exit");
}
void set_windows(WINDOW* output_window, WINDOW* username_window, WINDOW* input_window, WINDOW* app_name_window, std::deque<std::string>& messages) {
    //set these three windows
    //no need to set input_window
    set_output_window(output_window, messages);
    set_username_window(username_window);
    set_app_name_window(app_name_window);

    //set box
    setBox(output_window);
    setBox(username_window);
    setBox(input_window);
    setBox(app_name_window);
    
    //refresh the super window and all other windows.
    //move the cursor to input windows
    refresh();
    wrefresh(output_window);
    wrefresh(username_window);
    wrefresh(input_window);
    wrefresh(app_name_window);
    move(14, 2);
}


bool input(char msg[40], char *username, std::deque<std::string>& dq) {
    getstr(msg);
    std::string tstr(msg);
    if(tstr=="\\q") return false;
    if(dq.size()>=10) dq.pop_front();
    dq.push_back(tstr);
    auto ptr = getChatLog();
    ChatInfo chatInfo(username, msg);
    ptr->push(chatInfo);
    move(14, 2);
    return true;
}