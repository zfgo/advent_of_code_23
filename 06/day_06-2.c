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

long long int get_win_combos(long long int time, long long int record)
{
    long long int race_dist, i;
    long long int combos = 0;

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

long long int calc(long long int time, long long int rec)
{
    long long int total = 1;
    total *= get_win_combos(time, rec);

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

    long long int time;
    long long int rec;

    sscanf(lines[0], "Time: %lld", &time);
    sscanf(lines[1], "Distance: %lld", &rec);

    long long int total = calc(time, rec);
    printf("win possibilities: %lld\n", total);

    return 0;
}
