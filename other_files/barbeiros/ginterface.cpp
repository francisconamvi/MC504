#include <ncurses.h>
using namespace std;

void init(){
    // initscr();
    refresh();
    move(1,40);

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    mvprintw(1, xMax/2, "Atelie foi aberto!!!");
    refresh();
    return;
}