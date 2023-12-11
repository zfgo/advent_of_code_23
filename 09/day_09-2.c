#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LENGTH 16
#define INPUT_LEN 21

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

void parse_lines(char *lines[], int **input, int n)
{
    int i = 0;

    for (i = 0; i < n; ++i)
    {
        sscanf(lines[i], "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                &input[i][0], &input[i][1], &input[i][2], &input[i][3], 
                &input[i][4], &input[i][5], &input[i][6], &input[i][7], 
                &input[i][8], &input[i][9], &input[i][10], &input[i][11], 
                &input[i][12], &input[i][13], &input[i][14], &input[i][15], 
                &input[i][16], &input[i][17], &input[i][18], &input[i][19], 
                &input[i][20]);
    }
}

int predict_first_number(int *arr,  int len)
{
    int next_arr[len-1];
    int running_total = 0;
    int i;

    for (i = 0; i < len-1; ++i)
    {
        next_arr[i] = arr[i+1] - arr[i];
        running_total += next_arr[i];
        // printf("%d ", next_arr[i]);
    }
    // printf("\n");

    if (running_total == 0) // base case: we have found the "all-zero history"
    {
        return arr[0] - 0;
    }
    else 
    {
        return arr[0] - predict_first_number(next_arr, len-1);
    }
}

int calc(int **input, int n)
{
    int total = 0;
    int i;

    for (i = 0; i < n; ++i)
    {
        /*
        for (int j = 0; j < INPUT_LEN; ++j)
        {
            printf("%d ", input[i][j]);
        }
        printf("\n");
        */
        int first = predict_first_number(input[i], INPUT_LEN);
        total += first;
        //printf("first: %d\n", first);
    }
    return total;
}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;
    int **input;

    line = read_line(WORD_LENGTH);

    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    input = (int **)malloc(sizeof(int *) * n);
    for (int i = 0; i < n; ++i)
    {
        input[i] = (int *)malloc(sizeof(int) * INPUT_LEN);
    }

    parse_lines(lines, input, n);
    int total = calc(input, n);
    printf("the total is: %d\n", total);

    for (int i = 0; i < n; ++i)
    {
        free(lines[i]);
        free(input[i]);
    }
    free(input);
    free(line);

    return 0;
}
