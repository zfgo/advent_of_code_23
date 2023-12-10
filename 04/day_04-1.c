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

int get_card_score(int winners[], int numbers[])
{
    int score = 0;
    int i, j;
    /*
    printf("winners: ");
    for (i = 0; i < 10; ++i)
    {
        printf("%d ", winners[i]);
    }
    printf("\n");
    */

    for (i = 0; i < 25; ++i)
    {
        for (j = 0; j < 10; ++j)
        {
            if (numbers[i] == winners[j])
            {
                // we have a winner!
                score = (score == 0) ? 1 : score * 2;
                printf("found a winner! %d\n", numbers[i]);
                break;
            }
        }
    }
    // printf("score: %d\n", score);
    return score;
}

int parse_lines(char *lines[], int n)
{
    int winners[10];
    int numbers[25];
    char *start;
    int total_score = 0;
    
    for (int i = 0; i < n; ++i)
    {
        int j = 0;

        start = strchr(lines[i], ':');
        char *token = strtok(start+1, " ");

        while (token != NULL)
        {
            if (j < 10)
            {
                winners[j] = atoi(token);
            }
            else if (j >= 11)
            {
                numbers[j-11] = atoi(token);
            }

            token = strtok(NULL, " ");
            ++j;
        }
        
        total_score += get_card_score(winners, numbers);
    }
    return total_score;
}

int main()
{
    int n = 0;
    char *lines[1024];
    char *line;
    int score;

    line = read_line(WORD_LENGTH);

    while (*line)
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    score = parse_lines(lines, n);
    printf("total score: %d\n", score);

    return 0;
}
