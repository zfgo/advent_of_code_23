#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LENGTH 16
#define NUM_BITS 10
#define HASH_UNKNOWN 2048
#define ACCEPTED 2049
#define REJECTED 2050
#define DOES_NOT_EXIST 2051

#define hashsize(n) ((unsigned long)1 << (n))
#define hashmask(n) (hashsize(n) - 1)

typedef enum cond 
{
    LT = 0,
    GT = 1,
    NA = -1
} Cond;

typedef struct rule
{
    char name[8];
    int x_i, m_i, a_i, s_i;     // counters for each rule in the workflow
    int x[8], m[8], a[8], s[8]; // each x, m, a, s can have multiple rules in 1 workflow
    Cond x_c[8], m_c[8], a_c[8], s_c[8]; // conditions for each rule (see the enum cond)

    // rules: goto this rule:
    //  'A' for accepted
    //  'R' for rejected
    //  <rule->name> if another rule is needed
    char x_r[8][8], m_r[8][8], a_r[8][8], s_r[8][8];

    // hashes of the other rules
    //  <rule_name> : hash
    //  'A' : ACCEPTED (2049)
    //  'R' : REJECTED (2050)
    //  doesn't exist : DOES_NOT_EXIST (2051)
    unsigned long int x_h[8], m_h[8], a_h[8], s_h[8];
    unsigned long int hash; // it might be convenient to store the workflows in 
                            // a hash table based on the name of the rule
    char base[8];    // default (or `base') rule
    char base_h;     // hash of the default rule
    struct rule *next;
} Rule;

typedef struct part
{
    int x, m, a, s;
    bool accepted;
} Part;

void print_rule(Rule *r)
{
    printf("%s (", r->name);
    for (int i = 0; i < 8; ++i)
    {
        if (i < r->x_i)
        {
            switch (r->x_c[i])
            {
                case LT:
                    printf("x<%d: %s, ", r->x[i], r->x_r[i]);
                    break;
                case GT:
                    printf("x>%d: %s, ", r->x[i], r->x_r[i]);
                    break;
            }
        }
        if (i < r->m_i)
        {
            switch (r->m_c[i])
            {
                case LT:
                    printf("m<%d: %s, ", r->m[i], r->m_r[i]);
                    break;
                case GT:
                    printf("m>%d: %s, ", r->m[i], r->m_r[i]);
                    break;
            }
        }
        if (i < r->a_i)
        {
            switch (r->a_c[i])
            {
                case LT:
                    printf("a<%d: %s, ", r->a[i], r->a_r[i]);
                    break;
                case GT:
                    printf("a>%d: %s, ", r->a[i], r->a_r[i]);
                    break;
            }
        }
        if (i < r->s_i)
        {
            switch (r->s_c[i])
            {
                case LT:
                    printf("s<%d: %s, ", r->s[i], r->s_r[i]);
                    break;
                case GT:
                    printf("s>%d: %s, ", r->s[i], r->s_r[i]);
                    break;
            }
        }
    }
    printf(") hash: %llu\n", r->hash);
}

void print_part(Part *p)
{
    printf("x: %d, m: %d, a: %d, s: %d, acc: %s\n", p->x, p->m, p->a, p->s, 
            p->accepted ? "true" : "false");
}

unsigned long oaat(char *key, unsigned long len, unsigned long bits) 
{
    unsigned long hash, i;

    if (len == 1)
    {
        if (key[0] == 'A')
            return ACCEPTED;
        else if (key[0] == 'R')
            return REJECTED;
    }

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

void parse_lines(char *lines[], int n, Rule *rule_map[], Part parts[], int *n_parts)
{
    int i;

    for (i = 0; lines[i][0] != '{'; ++i)
    {
        // TODO: parse the rules, and put in the hash table
    }
    for (i = i+1; i < n; ++i)
    {
        *n_parts++;
        // TODO: parse the parts
    }


}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;
    static Rule *rule_map[1 << NUM_BITS] = {NULL};
    int n_parts;
    static Part parts[256];

    line = read_line(WORD_LENGTH);

    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    parse_lines(lines, n, rule_map, parts, &n_parts);

    return 0;
}
