#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LENGTH 16

typedef struct game 
{
    int r;
    int g;
    int b;
    int power;
} Game;

char *read_line(int size)
{
    char *str;
    int ch;
    int len = 0;

    str = (char *)malloc(size);
    if (str == NULL)
    {
        fprintf(stderr, "Malloc error, exiting\n");
        exit(1);
    }

    while ((ch = getchar()) != EOF && (ch != '\n'))
    {
        str[len++] = ch;
        if (len == size)
        {
            size = 2 * size;
            str = realloc(str, size);
            if (str == NULL)
            {
                fprintf(stderr, "Malloc error, exiting\n");
                exit(1);
            }
        }
    }
    str[len] = '\0';
    return str;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int get_color(char *ptr)
{
    //printf("getting color for %s", ptr);
    int i = 2;

    while (true)
    {
        if (ptr[i] == 'r')
        {
            return 12;
        }
        else if (ptr[i] == 'g')
        {
            return 13;
        }
        else if (ptr[i] == 'b')
        {
            return 14;
        }
        ++i;
    }

    // we should never reach this line
    return -1;
}

void parse_lines(char *lines[], Game *games, int n)
{
    int i; 
    char *start;
    char *token;
    int num, color;

    for (i = 0; i < n; ++i) 
    {
        start = strchr(lines[i], ':');
        token = strtok(start+1, " ");

        while (token != NULL)
        {
            num = atoi(token);
            color = get_color(token);

            if (color == 12) // red 
            {
                games[i].r = max(games[i].r, num);
            }
            else if (color == 13) // green 
            {
                games[i].g = max(games[i].g, num);
            }
            else if (color == 14) // blue 
            {
                games[i].b = max(games[i].b, num);
            }
            else
            {
                printf("a serious problem has occurred :(\n");
            }
            printf("DEBUG: parsed line %d", i); // IT ONLY WORKS WHEN THIS PRINT 
                                                // STATEMENT IS HERE ??????????
                                                // there has to be some weird memory error in
                                                // here but idk where or what it is, maybe 
                                                // something w strtok??? idk
            token = strtok(NULL, " ");
        }
    }
}

void calc_game_powers(Game *games, int n)
{
    int i;

    for (i = 0; i < n; ++i)
    {
        games[i].power = games[i].r * games[i].g * games[i].b;
    }
}

int calc_power_totals(Game *games, int n)
{
    int i;
    int total = 0;

    for (i = 0; i < n; ++i)
    {
        total += games[i].power;
    }

    return total;
}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;
    Game *games;

    line = read_line(WORD_LENGTH);

    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    games = (Game *)calloc(n, sizeof(Game));
    parse_lines(lines, games, n);
    calc_game_powers(games, n);
    int total = calc_power_totals(games, n);

    // int total = check_cubes(lines, n);

    printf("sum of game powers: %d\n", total);

    return 0;
}
