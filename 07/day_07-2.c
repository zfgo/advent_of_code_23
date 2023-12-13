#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LENGTH 16
#define HAND_LEN 5

char cards[16] = "J23456789TQKA";

typedef struct hand
{
    char hand[8];
    int bid;
    int type;
    int rank;
} Hand;

enum hand_type
{
    HIGH_CARD,
    ONE_PAIR,
    TWO_PAIR,
    THREE_KIND,
    FULL_HOUSE,
    FOUR_KIND,
    FIVE_KIND
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

void parse_lines(char *lines[], Hand *hands, int n)
{
    int i;
    
    for (i = 0; i < n; ++i)
    {
        sscanf(lines[i], "%s %d", hands[i].hand, &hands[i].bid);
    }
}

int calc_type(Hand h)
{
    char map[] = "00000";
    int cards_ind = 0;
    int count[HAND_LEN] = {0, 0, 0, 0, 0};
    int i, j;
    int joker_ct = 0;

    for (i = 0; i < HAND_LEN; ++i)
    {
        if (h.hand[i] == 'J')
        {
            joker_ct++;
            continue;
        }
        if (! strchr(map, h.hand[i]))       // check if the card is in our card counter string
        {
            map[cards_ind] = h.hand[i];     // if not, assign the next available index to that card
            cards_ind++;
        }
        for (j = 0; j < HAND_LEN; ++j)
        {
            if (h.hand[i] == map[j])
            {
                count[j]++;                 // and then increment the count of that card
                break;
            }
        }
    }
    
    // take care of jokers (I'm pretty sure it's always best to add them 
    // to the group of cards that is already largest
    int largest_ind = 0;
    int largest = count[0];

    for (i = 0; i < HAND_LEN; ++i)
    {
        if (count[i] > largest)
        {
            largest = count[i];
            largest_ind = i;
        }
    }
    count[largest_ind] += joker_ct;
    
    for (i = 0; i < HAND_LEN; ++i)
    {
        switch (count[i])
        {
            case 5:                             // one possibility: five kind
                return FIVE_KIND;
            case 4:                             // one possibility: four kind
                return FOUR_KIND;
            case 3:                             // two possibilities: full house, three kind
                for (j = 0; j < HAND_LEN; ++j)  // check for full house
                {
                    if (count[j] == 2)
                    {
                        return FULL_HOUSE;
                    }
                }
                return THREE_KIND;              // if full house not returned, return three kind (set)
            case 2:                             // three possibilities: full house, two_pair, one pair
                for (j = 0; j < HAND_LEN; ++j)
                {
                    if (count[j] == 3)          // check for full house
                    {
                        return FULL_HOUSE;
                    }
                    if (count[j] == 2 && i != j)// check for two pair
                    {
                        return TWO_PAIR;
                    }
                }
                return ONE_PAIR;                // otherwise one pair
        }
    }
    return HIGH_CARD;                           // base case: high card
}

void get_hand_types(Hand *hands, int n)
{
    int i;

    for (i = 0; i < n; ++i)
    {
        hands[i].type = calc_type(hands[i]);
    }
}

int get_card_strength(char c)
{
    int i, n;
    n = strlen(cards);

    for (i = 0; i < n; ++i)
    {
        if (c == cards[i])
        {
            return i;
        }
    }
    // WARNING: if we reach this line, something has gone very wrong
    return -1;
}

/*
 * comparison function for qsort(), very similar to the `compare_hands` function,
 * but with accepting "generic" void * inputs
 */
int compare_hands_v2(const void *x, const void *y)
{
    Hand *a = (Hand *)x;
    Hand *b = (Hand *)y;
    int i;

    if (a->type < b->type)
    {
        return -1;
    }
    else if (a->type == b->type)
    {
        for (i = 0; i < HAND_LEN; ++i)
        {
            int stren_a = get_card_strength(a->hand[i]);
            int stren_b = get_card_strength(b->hand[i]);
            
            if (stren_a < stren_b)
            {
                return -1;
            }
            else if (stren_a > stren_b)
            {
                return 1;
            }
            // if the card strengths are equal, continue to the next card to check strengths
        }
    }
    return 1;
}

/*
 * UNUSED 
 *
 * check if hand a is "less than" hand b
 */
bool compare_hands(Hand a, Hand b)
{
    int i;

    if (a.type < b.type)
    {
        return true;
    }
    else if (a.type == b.type)
    {
        for (i = 0; i < HAND_LEN; ++i)
        {
            int stren_a = get_card_strength(a.hand[i]);
            int stren_b = get_card_strength(b.hand[i]);
            
            if (stren_a < stren_b)
            {
                return true;
            }
            else if (stren_a > stren_b)
            {
                return false;
            }
            // if the card strengths are equal, continue to the next card to check strengths
        }
    }
    return false;
}

/* 
 * UNUSED 
 */
void swap_hands(Hand *a, Hand *b)
{
    Hand tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
 * UNUSED 
 *
 * selection sort implementation
 */
void sort_hands(Hand *hands, int n)
{
    int i, j, min;

    for (i = 0; i < n; ++i)
    {
        min = i;
        for (j = i + 1; j < n; ++j)
        {
            if (compare_hands(hands[j], hands[min])) // current hand is less than the previously smallest hand
            {
                // printf("hand %s is smaller thand hand %s !\n", hands[j].hand, hands[min].hand);
                min = j;
            }
        }
        swap_hands(&hands[min], &hands[i]);
    }
}

int calc_winnings(Hand *hands, int n)
{
    long long int total = 0;
    int i;

    for (i = 0; i < n; ++i)
    {
        // printf("hand: %s, type: %d, rank: %d\n", hands[i].hand, hands[i].type, i+1);
        total += (i + 1) * hands[i].bid;
    }
    return total;
}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;
    Hand *hands = NULL;

    line = read_line(WORD_LENGTH);

    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    hands = (Hand *)malloc(sizeof(Hand) * n);

    parse_lines(lines, hands, n);
    get_hand_types(hands, n);
    //sort_hands(hands, n);

    qsort(hands, n, sizeof(Hand), compare_hands_v2);    // use stdlib's built-in qsort implementation

    long long int winnings = calc_winnings(hands, n);
    printf("total winnings: %lld\n", winnings);

    for (int i = 0; i < n; ++i)
    {
        free(lines[i]);
    }
    free(hands);
    free(line);

    return 0;
}
