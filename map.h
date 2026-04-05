#ifndef _MAP_H_
#define _MAP_H_

typedef enum
{
    EASY = 1,
    MEDIUM,
    HARD,
    EXPERT
} Difficulty;

typedef struct
{
    int rows;
    int cols;
    int mines;
} Map;

typedef struct
{
    bool has_mine;
    bool revealed;
    bool flagged;
    int adjacent_mines;
} Cell;

static inline Map get_map(int level)
{
    switch (level)
    {
    case EASY:
        return (Map){9, 9, 10};
    case MEDIUM:
        return (Map){16, 16, 40};
    case HARD:
        return (Map){16, 30, 99};
    case EXPERT:
        return (Map){24, 30, 150};
    default:
        return (Map){9, 9, 10};
    }
}

#endif