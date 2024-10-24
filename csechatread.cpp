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
Most of the functions in csechatread does the same purpose as csechatwrite and
the overall procedure is very similar to csechatwrite.

There is one exception and that is set_username_window
*/

Users* getUsersPtr();
ChatLog* getChatLog();
void initial_setup(int, char* argv[]);
void set_windows(WINDOW*, WINDOW*, WINDOW*);
void set_output_window(WINDOW* window);

/*
set_username_window in csechatwrite is just about print the things that only that program has inputted.
In read program, its purpose is to access the ChatLog shared memory and print that.
*/
void set_username_window(WINDOW* window);
void set_app_name_window(WINDOW* window);
void setBox(WINDOW*);
void failed_memory_segment();
void failed_attach();

int main(int argc, char* argv[]) {
    //escape conditions
    initial_setup(argc, argv);

    WINDOW *output_window = newwin(output_row+6, output_column+2, 0, 0);
    WINDOW *username_window = newwin(output_row+2, 18, 0, output_column+3);
    WINDOW *app_name_window = newwin(3, 18, output_row+3, output_column+3);

    char quit = '\0';
    do {
        set_windows(output_window, username_window, app_name_window);
    } while((quit=getch())!='q');

    endwin();
    return 0;
}

void failed_memory_segment() {
    fprintf(stderr, "Error: Failed to create or get shared memory segment.");
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

void initial_setup(int argc, char* argv[]) {
    initscr();
    cbreak();
    curs_set(0);
    noecho();
    start_color();

    
    //check if the shared memory of user is fine
    auto shmUsersPtr = getUsersPtr();
    shmdt(shmUsersPtr);

    //check if the shared memory of chat is fine
    auto shmChatLogPtr = getChatLog();
    shmdt(shmChatLogPtr);

    timeout(50);
}

void setBox(WINDOW* window) {
    box(window, '*', '*');
}

void set_output_window(WINDOW* window) {
    auto ptr = getChatLog();
    werase(window);
    for(int i=0;i<ptr->counter;i++) {
        std::string str = ptr->chatInfos[i].getMessage();
        mvwprintw(window, i+1, 1, "%s\n", str.c_str());
    }
    shmdt(ptr);
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
    mvwprintw(window, 1, 1, "Enter q to Exit");
}

void set_windows(WINDOW* output_window, WINDOW* username_window, WINDOW* app_name_window) {
    set_output_window(output_window);
    set_username_window(username_window);
    set_app_name_window(app_name_window);
    setBox(output_window);
    setBox(username_window);
    setBox(app_name_window);
    
    refresh();
    wrefresh(output_window);
    wrefresh(username_window);
    wrefresh(app_name_window);
}