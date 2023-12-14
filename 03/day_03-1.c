#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LENGTH 16

typedef struct num 
{
    char word[8];
    int num;
    int row_ind;
    int l, r;
} Num;

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

void parse_lines(char *lines[], Num *nums, int n, int m, int *num_len);
{
    int i, j;
    int nums_ind = 0;
    int num = 0;
    int num_len = 0;
    bool is_dig = false;

    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < m; ++j)
        {
            if (nums_ind >= *num_len)
            {
                nums = (Num *)realloc(nums, sizeof(Num) * num_len * 2);
                *num_len *= 2;
            }
            if (isdigit(lines[i][j]))
            {
                num += lines[i][j] - '0';
                if (num_len)
                {
                    num *= 10;
                }
                num_len++;
            }
        }
    }
}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;
    Num *nums;

    line = read_line(WORD_LENGTH);

    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }
    
    int m = strlen(lines[0]);
    nums = (Num *)malloc(sizeof(Num) * n);
    int num_len = 1024;
    
    parse_lines(lines, nums, n, m, &num_len);

    int total = calc(lines, n);
    printf("total: %d\n", total);

    return 0;
}
