#include <curses.h>

int gameMenu(int *level)
{
    int y, x, y_center, x_center;
    getmaxyx(stdscr, y, x);
    x_center = (x / 2);
    y_center = (y / 2);

    WINDOW *creator = newwin(1, 20, y_center + 12, x_center + 15);
    wattron(creator, A_BOLD);
    wprintw(creator, "por Aurelio Sampaio");
    wattroff(creator, A_BOLD);
    wrefresh(creator);

    WINDOW *name = newwin(3, 15, y_center - 9, x_center - 30);
    wattron(name, A_BOLD);
    box(name, 0, 0);
    mvwprintw(name, 1, 1, " MINESWEEPER ");
    wattroff(name, A_BOLD);
    wrefresh(name);

    WINDOW *level_win = newwin(3, 15, y_center + 1, x_center - 30);
    wattron(level_win, A_BOLD);
    box(level_win, 0, 0);
    mvwprintw(level_win, 1, 1, "   level     ");
    wattroff(level_win, A_BOLD);
    wrefresh(level_win);

    WINDOW *menu_win = newwin(5, 11, y_center - 5, x_center - 30);
    box(menu_win, 0, 0);
    keypad(menu_win, TRUE);

    char *options[] = {"Play     ", "Settings ", "Exit     "};
    int highlight = 0, key;

    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            if (i == highlight)
                wattron(menu_win, A_REVERSE);
            if (i == 0 && highlight == 0)
                mvwprintw(menu_win, i + 1, 1, "<Play   >");
            else
                mvwprintw(menu_win, i + 1, 1, options[i]);
            wattroff(menu_win, A_REVERSE);
        }
        wrefresh(menu_win);

        if (highlight == 0)
            mvwprintw(level_win, 1, 10, "%d", *level);
        wrefresh(level_win);

        key = wgetch(menu_win);

        switch (key)
        {
        case KEY_LEFT:
            if (*level != 1)
                (*level)--;
            break;

        case KEY_RIGHT:
            if (*level != 4)
                (*level)++;
            break;

        case KEY_UP:
            if (highlight != 0)
                highlight--;
            break;

        case KEY_DOWN:
            if (highlight != 2)
                highlight++;
            break;
        }
        if (key == 10 || key == 'a')
            break;
    }
    wclear(level_win);
    wrefresh(level_win);
    delwin(level_win);
    wclear(creator);
    wrefresh(creator);
    delwin(creator);
    wclear(name);
    wrefresh(name);
    delwin(name);
    wclear(menu_win);
    wrefresh(menu_win);
    delwin(menu_win);
    refresh();
    return highlight;
}