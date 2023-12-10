#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define WORD_LENGTH 16

typedef struct card 
{
    int winners[10];
    int numbers[25];
    int score;
    int matches; // same as the card's score
    int copies;
} Card;

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

int get_card_matches(Card *card)
{
    int matches = 0;
    int i, j;

    for (i = 0; i < 25; ++i)
    {
        for (j = 0; j < 10; ++j)
        {
            if (card->numbers[i] == card->winners[j])
            {
                ++matches;
                break;
            }
        }
    }
    return matches;
}

int get_card_score(Card *card)
{
    int score = 0;
    int i, j;

    for (i = 0; i < 25; ++i)
    {
        for (j = 0; j < 10; ++j)
        {
            if (card->numbers[i] == card->winners[j])
            {
                // we have a winner!
                score = (score == 0) ? 1 : score * 2;
                break;
            }
        }
    }
    return score;
}

void parse_lines(char *lines[], Card *cards, int n)
{
    char *start;
    
    for (int i = 0; i < n; ++i)
    {
        int j = 0;

        start = strchr(lines[i], ':');
        char *token = strtok(start+1, " ");

        while (token != NULL)
        {
            if (j < 10)
            {
                cards[i].winners[j] = atoi(token);
            }
            else if (j >= 11)
            {
                cards[i].numbers[j-11] = atoi(token);
            }

            token = strtok(NULL, " ");
            ++j;
        }

        // cards[i].score = get_card_score(&cards[i]);
        cards[i].matches = get_card_matches(&cards[i]);
    }
}

int count_games(Card *cards, int n)
{
    int i, j, k;
    int total_games = 0;

    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < (cards[i].copies + 1); ++j)
        {
            for (k = 0; k < cards[i].matches; ++k)
            {
                if ((i + 1 + k) < n)
                {
                    cards[i+1+k].copies += 1;
                }
                else 
                {
                    break;
                }
            }
        }
        //printf("card i (%d) matches: %d, copies: %d\n", i, cards[i].matches, cards[i].copies);
    }
    
    for (i = 0; i < n; ++i)
    {
        total_games += 1 + cards[i].copies;
    }

    return total_games;
}

int main()
{
    int n = 0;
    char *lines[1024];
    char *line;
    int n_games;

    line = read_line(WORD_LENGTH);

    while (*line)
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    Card *cards = NULL;
    cards = (Card *)malloc(sizeof(Card) * n);
    assert(cards);

    // int game_counter[n];

    for (int i = 0; i < n; ++i)
    {
        cards[i].matches = 0;
        cards[i].copies = 0;

        //game_counter[i] = 0;
    }

    parse_lines(lines, cards, n);
    n_games = count_games(cards, n);
    printf("total games played: %d\n", n_games);

    return 0;
}
