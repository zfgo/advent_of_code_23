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

int calc(char *lines[], int n)
{
    int i, j, len;
    len = strlen(lines[0]) - 1; // minus 1 so we don't mess with the newline

    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < len; ++j)
        {
            if (isdigit(lines[i][j]))
            {
                // do something
            }
        }
    }

    return 0;
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

    int total = calc(lines, n);
    printf("total: %d\n", total);

    return 0;
}
