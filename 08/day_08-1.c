#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LENGTH 16
#define NODE_WORD_LEN 3
#define NUM_BITS 10
#define HASH_UNKNOWN 2048

#define hashsize(n) ((unsigned long)1 << (n))
#define hashmask(n) (hashsize(n) - 1)

typedef struct node
{
    char name[4];       // node name
    unsigned long hash; // node hash
    unsigned long l, r; // hashes to left and right nodes
    char lw[4];
    char rw[4];
    struct node *next;
} Node;

void print_node(Node *n)
{
    printf("%s = (%s, %s)\n%lu = (%lu, %lu)\n", n->name, n->lw, n->rw, n->hash, n->l, n->r);
}

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

unsigned long oaat(char *key, unsigned long len, unsigned long bits) 
{
    unsigned long hash, i;

    for (hash = 0, i = 0; i < len; i++) 
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash & hashmask(bits);
}

void parse_lines(char *lines[], int n, Node *map[], char *instructions, int *instr_len)
{
    int i; 
    Node *cur;

    strcpy(instructions, lines[0]);
    *instr_len = strlen(instructions);

    for (i = 1; i < n; ++i)
    {
        cur = (Node *)malloc(sizeof(Node));
        if (cur == NULL)
        {
            fprintf(stderr, "Malloc error, exiting\n");
            exit(1);
        }

        sscanf(lines[i], "%3s = (%3s, %3s)", cur->name, cur->lw, cur->rw);
        cur->l = oaat(cur->lw, 3, NUM_BITS);
        cur->r = oaat(cur->rw, 3, NUM_BITS);
        cur->hash = oaat(cur->name, 3, NUM_BITS);

        cur->next = map[cur->hash];
        map[cur->hash] = cur;
    }
}

Node *get_node(Node *map[], char to_get[], unsigned long hash)
{
    unsigned long h = hash;
    if (h == HASH_UNKNOWN)
    {
        h = oaat(to_get, 3, NUM_BITS);
    }

    Node *n = map[h];
    while (strcmp(n->name, to_get) != 0)
    {
        n = n->next;
    }
    return n;
}

int calc_steps(Node *map[], char *instructions, int instr_len, char start[], char end[])
{
    Node *cur = get_node(map, start, HASH_UNKNOWN);
    Node *goal = get_node(map, end, HASH_UNKNOWN);

    //unsigned long cur = oaat(start, 3, NUM_BITS);
    //unsigned long goal = oaat(end, 3, NUM_BITS);
    int i = 0;

    for ( ; ; )
    {
        if (cur == goal)    // check if the current node is the goal node
        {
            return i;
        }
        if (instructions[i % instr_len] == 'L') // get next node
        {
            cur = get_node(map, cur->lw, cur->l);
        }
        else
        {
            cur = get_node(map, cur->rw, cur->r);
        }
        ++i;
    }

    // WARNING: if we reach this line, something has gone very wrong
    return -1;
}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;
    static Node *map[1 << NUM_BITS] = {NULL};
    char instructions[300];
    int instr_len;

    line = read_line(WORD_LENGTH);

    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    parse_lines(lines, n, map, instructions, &instr_len);
    int n_steps = calc_steps(map, instructions, instr_len, "AAA", "ZZZ");

    printf("number of steps: %d\n", n_steps);

    return 0;
}
