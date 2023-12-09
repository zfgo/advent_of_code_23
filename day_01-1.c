#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LINES 1024
#define WORD_LENGTH 16

const char *numbers[] = {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};

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

int calibrate(char *word_list[], int n)
{
    int i, j;
    int first, second;
    int calibration_value = 0;

    first = second = -1;

    for (i = 0; i < n; ++i)
    {
        j = 0;
        while (first == -1)
        {
           if (isdigit(word_list[i][j]))
           {
               first = word_list[i][j] - '0';
           }
           ++j;
        }

        while (word_list[i][j] != '\0')
        {
           if (isdigit(word_list[i][j]))
           {
               second = word_list[i][j] - '0';
           }
           ++j;
        }

        if (second != -1)
        {
            calibration_value += first * 10 + second;
        }
        else
        {
            calibration_value += first * 10 + first;
        }
        //printf("first: %d, second: %d, calibration_value: %d\n", first, second, calibration_value);

        first = second = -1;
    }

    return calibration_value;
}

int main(void)
{
    char *word;
    char *word_list[1024];
    int n = 0;
    int calibration_value;

    // input processing
    word = read_line(WORD_LENGTH);

    while (*word) // word != NULL
    {
        word_list[n] = word;
        word = read_line(WORD_LENGTH);
        ++n;
    }

    calibration_value = calibrate(word_list, n);
    printf("calibration_value: %d\n", calibration_value);
    
    return 0;
}
