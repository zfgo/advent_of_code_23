#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LENGTH 16

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

int get_win_combos(int time, int record)
{
    int race_dist, i;
    int combos = 0;

    for (i = 0; i < time; ++i)
    {
        race_dist = i * (time - i);
        if (race_dist > record)
        {
            ++combos;
        }
    }
    return combos;
}

int calc(int times[], int distances[])
{
    int total = 1;

    for (int i = 0; i < 4; ++i)
    {
        total *= get_win_combos(times[i], distances[i]);
    }

    return total;
}

int main()
{
    char *line;
    char *lines[8];
    int n = 0;

    line = read_line(WORD_LENGTH);

    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    int times[4];
    int distances[4];

    sscanf(lines[0], "Time: %d %d %d %d", 
            &times[0], &times[1], &times[2], &times[3]);
    sscanf(lines[1], "Distance: %d %d %d %d", 
            &distances[0], &distances[1], &distances[2], &distances[3]);

    int total = calc(times, distances);
    printf("win possibilities: %d\n", total);

    return 0;
}
