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

int get_color(char *ptr)
{
    if (ptr[3] == 'r')
    {
        return 12;
    }
    else if (ptr[3] == 'g')
    {
        return 13;
    }
    else if (ptr[3] == 'b')
    {
        return 14;
    }

    // we should never reach this line
    return -1;
}

bool parse_line(char line[])
{
    int i = 0;
    char *ptr = strchr(line, ':');

    while (ptr[i] != '\0')
    {
        if (isdigit(ptr[i]) && isdigit(ptr[i+1]))
        {
            int d = (ptr[i] - '0') * 10 + (ptr[i+1] - '0');
            if (d >= 12)
            {
                int c = get_color(ptr + i);
                /*
                printf("d: %d\n", d);
                */
                if (d > c)
                {
                    return false;
                }
            }
        }
        ++i;
    }

    return true;
}

int check_cubes(char *lines[], int n)
{
    int i;
    int sum = 0;

    for (i = 0; i < n; ++i)
    {
        if (parse_line(lines[i]))
        {
            sum += i + 1;
        }
    }

    return sum;
}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;

    line = read_line(WORD_LENGTH);

    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    int total = check_cubes(lines, n);

    printf("sum of IDs: %d\n", total);

    return 0;
}
