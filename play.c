#include <stdlib.h>
#include <windows.h>
#include <curses.h>
#include <time.h>
#include <stdbool.h>

HANDLE hInput;
DWORD old_mode;

typedef struct
{
    bool has_mine;
    bool revealed;
    bool flagged;
    int adjacent_mines;
} Cell;

typedef struct
{
    int rows;
    int cols;
    int mines;
} Map;

struct
{
    int x, y;
    bool win;
    bool lose;
} cursor = {0, 0, false, false};

Map get_map(int level)
{
    switch (level)
    {
    case 1:
        return (Map){9, 9, 10};
    case 2:
        return (Map){16, 16, 40};
    case 3:
        return (Map){16, 30, 99};
    case 4:
        return (Map){24, 30, 150};
    default:
        return (Map){9, 9, 10};
    }
}

Cell **allocate_grid(int rows, int cols)
{
    Cell **grid = malloc(rows * sizeof(Cell *));
    for (int i = 0; i < rows; i++)
        grid[i] = calloc(cols, sizeof(Cell));
    return grid;
}

void free_grid(Cell **grid, int rows)
{
    for (int i = 0; i < rows; i++)
        free(grid[i]);
    free(grid);
}

void place_mines(Cell **grid, Map map)
{
    int placed = 0;
    srand(time(NULL));
    while (placed < map.mines)
    {
        int r = rand() % map.rows;
        int c = rand() % map.cols;
        if (!grid[r][c].has_mine)
        {
            grid[r][c].has_mine = true;
            placed++;
        }
    }
}

void calculate_adjacent(Cell **grid, Map map)
{
    for (int r = 0; r < map.rows; r++)
    {
        for (int c = 0; c < map.cols; c++)
        {
            if (grid[r][c].has_mine)
            {
                grid[r][c].adjacent_mines = -1;
                continue;
            }
            int count = 0;
            for (int dr = -1; dr <= 1; dr++)
                for (int dc = -1; dc <= 1; dc++)
                    if (!(dr == 0 && dc == 0))
                    {
                        int nr = r + dr, nc = c + dc;
                        if (nr >= 0 && nr < map.rows && nc >= 0 && nc < map.cols)
                            if (grid[nr][nc].has_mine)
                                count++;
                    }
            grid[r][c].adjacent_mines = count;
        }
    }
}

void dig(Cell **grid, Map map, int r, int c)
{
    if (r < 0 || r >= map.rows || c < 0 || c >= map.cols)
        return;
    Cell *cell = &grid[r][c];
    if (cell->revealed || cell->flagged)
        return;
    cell->revealed = true;
    if (cell->has_mine)
        return;
    if (cell->adjacent_mines == 0)
    {
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++)
                if (!(dr == 0 && dc == 0))
                    dig(grid, map, r + dr, c + dc);
    }
}

void toggle_flag(Cell **grid, Map map, int r, int c)
{
    Cell *cell = &grid[r][c];
    if (!cell->revealed)
        cell->flagged = !cell->flagged;
}

bool check_win(Cell **grid, Map map)
{
    for (int r = 0; r < map.rows; r++)
        for (int c = 0; c < map.cols; c++)
            if (!grid[r][c].has_mine && !grid[r][c].revealed)
                return false;
    return true;
}

bool check_lose(Cell **grid, Map map)
{
    for (int r = 0; r < map.rows; r++)
        for (int c = 0; c < map.cols; c++)
            if (grid[r][c].has_mine && grid[r][c].revealed)
                return true;
    return false;
}

void draw(WINDOW *win, Cell **grid, Map map)
{
    werase(win);
    for (int r = 0; r < map.rows; r++)
    {
        for (int c = 0; c < map.cols; c++)
        {
            if (grid[r][c].revealed)
            {
                if (grid[r][c].has_mine)
                    mvwaddch(win, r, c, '*');
                else if (grid[r][c].adjacent_mines == 0)
                    mvwaddch(win, r, c, ' ');
                else
                    mvwaddch(win, r, c, '0' + grid[r][c].adjacent_mines);
            }
            else
            {
                if (grid[r][c].flagged)
                    mvwaddch(win, r, c, '$');
                else
                    mvwaddch(win, r, c, '#');
            }
        }
    }
    wrefresh(win);
}

bool get_window_coords(WINDOW *win, COORD mouse, int *row, int *col)
{
    int y, x;
    getbegyx(win, y, x);
    if (mouse.Y >= y && mouse.Y < y + getmaxy(win) &&
        mouse.X >= x && mouse.X < x + getmaxx(win))
    {
        *row = mouse.Y - y;
        *col = mouse.X - x;
        return true;
    }
    return false;
}

void handle_input(Cell **grid, Map map, WINDOW *win)
{
    INPUT_RECORD input;
    DWORD count;
    ReadConsoleInput(hInput, &input, 1, &count);
    if (input.EventType == MOUSE_EVENT)
    {
        MOUSE_EVENT_RECORD mer = input.Event.MouseEvent;
        int r, c;
        if (get_window_coords(win, mer.dwMousePosition, &r, &c))
        {
            if (mer.dwEventFlags == 0 && (mer.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED))
                dig(grid, map, r, c);
            else if (mer.dwEventFlags == 0 && (mer.dwButtonState & RIGHTMOST_BUTTON_PRESSED))
                toggle_flag(grid, map, r, c);
        }
    }
}

int end_window(WINDOW *game, WINDOW *win, const char *msg)
{
    box(win, 0, 0);
    mvwprintw(win, 1, 2, msg);
    mvwprintw(win, 3, 2, "Play Again?");
    mvwprintw(win, 4, 4, "(A) Yes");
    mvwprintw(win, 5, 4, "(S) No");
    wrefresh(win);
    while (1)
    {
        int ch = getch();
        if (ch == 'a')
            return 1;
        if (ch == 's')
            return 0;
    }
}

int play(int level)
{
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    cursor.x = cursor.y = 0;
    cursor.win = cursor.lose = false;

    int y, x, yc, xc;
    getmaxyx(stdscr, y, x);
    yc = y / 2;
    xc = x / 2;

    Map map = get_map(level);
    Cell **grid = allocate_grid(map.rows, map.cols);
    place_mines(grid, map);
    calculate_adjacent(grid, map);

    WINDOW *game = newwin(map.rows, map.cols, yc - map.rows / 2, xc - map.cols / 2);
    WINDOW *win = newwin(7, 20, yc - 3, xc - 10);

    // Setup console mouse input
    hInput = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hInput, &old_mode);
    SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);

    while (!cursor.win && !cursor.lose)
    {
        handle_input(grid, map, game);
        cursor.lose = check_lose(grid, map);
        cursor.win = check_win(grid, map);
        draw(game, grid, map);
    }

    int retry;
    if (cursor.win)
        retry = end_window(game, win, "YOU WON");
    else
        retry = end_window(game, win, "GAME OVER");

    free_grid(grid, map.rows);
    delwin(game);
    delwin(win);
    return retry ? play(level) : 0;

    endwin();
    return 0;
}