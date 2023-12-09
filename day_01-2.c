#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LINES 1024
#define WORD_LENGTH 16
#define N_NUMBERS 9

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

int find_first(char word[])
{
    int first = -1;
    int i, j;
    int smallest_digit_ind;
    char *ptr;

    i = 0;
    while (first == -1)
    {
        if (isdigit(word[i]))
        {
            first = word[i] - '0';
            smallest_digit_ind = i;
        }
        ++i;
    }

    for (i = 0; i < N_NUMBERS; ++i)
    {
        if ((ptr = strstr(word, numbers[i])) == NULL)
        {
            continue;
        }
        for (j = 0; j < smallest_digit_ind; ++j)
        {
            if (ptr == &word[j] && j < smallest_digit_ind)
            {
                smallest_digit_ind = j;
                first = i + 1;
                /*
                printf("in find_first: first: %d\n", first);
                printf("in find_first: smallest_digit_ind: %d\n", smallest_digit_ind);
                */
            }
        }
    }

    return first;
}

bool check_if_word_repeats(char word[], int i)
{
    return strstr(word, numbers[i]) != NULL;
}

int find_last(char word[])
{
    int last = -1;
    int largest_digit_ind;
    int i, j;
    char *ptr;

    i = 0;
    while (word[i] != '\0')
    {
        if (isdigit(word[i]))
        {
            last = word[i] - '0';
            largest_digit_ind = i;
        }
        ++i;
    }

    // case where there is no number 1-9 in the word
    if (last == -1)
    {
        largest_digit_ind = 0;
    }

    // now check for 
    for (i = 0; i < N_NUMBERS; ++i)
    {
        if ((ptr = strstr(&word[largest_digit_ind], numbers[i])) == NULL)
        {
            continue;
        }
        for (j = 0; j < strlen(word); ++j)
        {
            //printf("j: %d, largest ind: %d\n", j, largest_digit_ind);
            if (ptr == &word[j] && j > largest_digit_ind)
            {
                // case where a spelled out digit repeats in the word
                if (check_if_word_repeats(&word[j+1], i))
                {
                    ptr = strstr(&word[j+1], numbers[i]);
                    continue;
                }
                largest_digit_ind = j;
                last = i + 1;
            }
        }
    }
    /*
    printf("in find_last: last: %d\n", last);
    printf("in find_last: largest_digit_ind: %d\n", largest_digit_ind);
    */

    return last;
}

int calibrate(char *word_list[], int n)
{
    int first, last;
    int calibration_value = 0;

    for (int i = 0; i < n; ++i)
    {
        first = find_first(word_list[i]);
        last = find_last(word_list[i]);
        /*
        printf("first: %d\n", first);
        printf("last: %d\n", last);
        */

        calibration_value += first * 10 + last;
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
    printf("calibration value: %d\n", calibration_value);
    
    return 0;
}
