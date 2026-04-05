#include <curses.h>
#include <stdlib.h>
#include "menu.h"
#include "play.h"

int initEngine()
{
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();
}

int main()
{
    initEngine();

    int menu, level = 1;
    bool running = TRUE;

    while (running)
    {
        menu = gameMenu(&level);

        switch (menu)
        {
        case 0:
            play(level);
            break;

        case 1:
            break;

        case 2:
            running = FALSE;
            break;
        }
    }

    getch();

    endwin();
    return 0;
}