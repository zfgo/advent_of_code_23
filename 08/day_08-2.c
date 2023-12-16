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

long long int gcd(long long int a, long long int b)
{
    if (b == 0)
    {
        return a;
    }
    return gcd(b, a % b);
}

long long int lcm(long long int a, long long int b)
{
    if (a > b)
    {
        return (a / gcd(a, b)) * b;
    }
    return (b / gcd(a, b)) * a;
}

long long int calc_steps(Node *map[], char *instructions, int instr_len)
{
    // Node *cur = get_node(map, start, HASH_UNKNOWN);
    // Node *goal = get_node(map, end, HASH_UNKNOWN);

    //unsigned long cur = oaat(start, 3, NUM_BITS);
    //unsigned long goal = oaat(end, 3, NUM_BITS);
    long long int i = 0;
    long long int total = 1;
    int j;
    int n_end_A = 0;
    Node *cur_nodes[32] = {NULL};
    long long int steps_to_z[32];
    Node *tmp;

    for (i = 0; i < 1 << NUM_BITS; ++i)     // get all the nodes that end in 'A'
    {
        tmp = map[i];
        while (tmp != NULL)
        {
            if (tmp->name[2] == 'A')
            {
                cur_nodes[n_end_A++] = tmp;
            }
            tmp = tmp->next;
        }
    }

    for (j = 0; j < n_end_A; ++j)
    {
        i = 0;
        for ( ; ; )
        {
            if (cur_nodes[j]->name[2] == 'Z')    // check if the current node is the goal node
            {
                steps_to_z[j] = i;
                break;
            }
            if (instructions[i % instr_len] == 'L') // get next instruction
            {
                cur_nodes[j] = get_node(map, cur_nodes[j]->lw, cur_nodes[j]->l);
            }
            else
            {
                cur_nodes[j] = get_node(map, cur_nodes[j]->rw, cur_nodes[j]->r);
            }
            ++i;
        }
    }

    for (i = 0; i < n_end_A; ++i)
    {
        total = lcm(total, steps_to_z[i]);  // LCM trick that I found on the subreddit
                                            // without this it will take a very long 
                                            // time to calculate the number of steps
    }

    return total;
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

    while (*line) // line[0] != '\0'
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    parse_lines(lines, n, map, instructions, &instr_len);
    long long int n_steps = calc_steps(map, instructions, instr_len);

    printf("number of steps: %lld\n", n_steps);

    for (int i = 0; i < 1 << NUM_BITS; ++i)
    {
        Node *cur = map[i];
        while (cur != NULL)
        {
            Node *tmp = cur;
            cur = tmp->next;
            free(tmp);
        }
    }

    for (int i = 0; i < n; ++i)
    {
        free(lines[i]);
    }
    free(line);

    return 0;
}
