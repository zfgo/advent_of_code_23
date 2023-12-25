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

#define MIN_RATING 1
#define MAX_RATING 4000

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
    char xmas;      // which part rating needs to be compared [xmas]
    int val;        // compare the [xmas] rating to this value
    Cond cond;      // condition (<, >)
    bool is_base;
    char wf[8];     // name of the workflow to goto OR 'A' or 'R'
    struct rule *l; // for creating a binary tree of rules
    struct rule *r;
} Rule;

typedef struct workflow
{
    char name[8];
    char xmas[8];
    int val[8];
    Cond cond[8];
    int i;

    // rules: goto this rule:
    //  'A' for accepted
    //  'R' for rejected
    //  <rule->name> if another rule is needed
    char wf[8][8];

    // hashes of the other rules
    //  <rule_name> : hash
    //  'A' : ACCEPTED (2049)
    //  'R' : REJECTED (2050)
    //  doesn't exist : DOES_NOT_EXIST (2051)
    unsigned long int wf_h[8];
    unsigned long int hash; // it might be convenient to store the workflows in 
                            // a hash table based on the name of the rule
    char base[8];    // default (or `base') rule
    unsigned long int base_h;     // hash of the default rule
    struct workflow *next;
} Workflow;

typedef struct range
{
    int min, max;
    //int max_x, max_m, max_a, max_s;
} Range;

typedef struct part
{
    Range x, m, a, s;
    long long int possible_ratings;
    bool accepted;
} Part;

void print_rule(Workflow *wf)
{
    printf("%s (", wf->name);
    for (int i = 0; i < wf->i; ++i)
    {
        printf("%c%c%d: %s, ", wf->xmas[i], (wf->cond[i] == LT) ? '<' : '>', 
                wf->val[i], wf->wf[i]);
    }
    printf("base: %s) hash: %lu, n_rules: %d\n", wf->base, wf->hash, wf->i);
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

void parse_lines(char *lines[], int n, Workflow *wf_map[])
{
    int i;
    //int j;
    char name[8];
    char rules[64];
    Workflow *wf;

    for (i = 0; lines[i][0] != '{'; ++i)
    {
        wf = (Workflow *)malloc(sizeof(Workflow));
        wf->i = 0;

        // scan in the line into name and wf bufs
        sscanf(lines[i], "%[a-z]{%[a-zA-Z0-9:<>,]}", name, rules);
        strcpy(wf->name, name);  // set the rule's name
        wf->hash = oaat(name, strlen(name), NUM_BITS); // and get the rule's hash

        char *tok = strtok(rules, ","); // tokenize the wf based on ","

        while (tok != NULL)
        {
            if (strlen(tok) < 5) // this is the last instruction 
            {                    // (minimum non-last instruction size is 5)
                strcpy(wf->base, tok);
                wf->base_h = oaat(tok, strlen(tok), NUM_BITS);
            }
            else
            {
                char ltgt;

                sscanf(tok, "%c%c%d:%[a-zAR]", &wf->xmas[wf->i], &ltgt, 
                        &wf->val[wf->i], wf->wf[wf->i]);
                wf->cond[wf->i] = (ltgt == '<') ? LT : GT;
                wf->wf_h[wf->i] = oaat(wf->wf[wf->i], strlen(wf->wf[wf->i]), NUM_BITS);
                wf->i++;
            }
            tok = strtok(NULL, ",");
        }
        wf->next = wf_map[wf->hash];
        wf_map[wf->hash] = wf;
    }
}

bool check_rule(Cond c, int c_val, int p_val)
{
    switch (c)
    {
        case LT:
            return p_val < c_val;
        case GT:
            return p_val > c_val;
        default:
            fprintf(stderr, "Error, in the default case in `check_rule\n");
            return false;
    }
}

Workflow *get_workflow(Workflow *wf_map[], unsigned long int hash, char name[])
{
    Workflow *wf;
    if (hash > 1 << NUM_BITS)
        printf("about to get a rule for hash %lu\n", hash);
    wf = wf_map[hash];

    while (wf != NULL && strcmp(wf->name, name) != 0)
    {
        wf = wf->next;
    }

    return wf;
}

bool check_part_helper(Workflow *cur, Part *p, int i)
{
    if (i == cur->i) // base case 
    {
        switch (cur->base_h)
        {
            case ACCEPTED:
                p->accepted = true;
                return true;
            case REJECTED:
                p->accepted = false;
                return true;
            default:
                return false;
        }
    }

    switch (cur->wf_h[i])
    {
        case ACCEPTED:
            p->accepted = true;
            return true;
        case REJECTED:
            p->accepted = false;
            return true;
        default:
            return false;
    }
}

void check_part(Workflow *wf_map[], Part *p, unsigned long int in_h)
{
    int i;
    Workflow *cur;
    bool stay_on_WF;

    cur = wf_map[in_h];
    while (cur != NULL && strcmp(cur->name, "in") != 0)
    {
        cur = cur->next;
    }

    for ( ; ; )
    {
        stay_on_WF = true;
        for (i = 0; i < cur->i && stay_on_WF; ++i)
        {
            switch (cur->xmas[i])
            {
                case 'x':
                    if (check_rule(cur->cond[i], cur->val[i], p->x))
                    {
                        if (check_part_helper(cur, p, i))
                            return;
                        cur = get_workflow(wf_map, cur->wf_h[i], cur->wf[i]);
                        stay_on_WF = false;
                    }
                    break;
                case 'm':
                    if (check_rule(cur->cond[i], cur->val[i], p->m))
                    {
                        if (check_part_helper(cur, p, i))
                            return;
                        cur = get_workflow(wf_map, cur->wf_h[i], cur->wf[i]);
                        stay_on_WF = false;
                    }
                    break;
                case 'a':
                    if (check_rule(cur->cond[i], cur->val[i], p->a))
                    {
                        if (check_part_helper(cur, p, i))
                            return;
                        cur = get_workflow(wf_map, cur->wf_h[i], cur->wf[i]);
                        stay_on_WF = false;
                    }
                    break;
                case 's':
                    if (check_rule(cur->cond[i], cur->val[i], p->s))
                    {
                        if (check_part_helper(cur, p, i))
                            return;
                        cur = get_workflow(wf_map, cur->wf_h[i], cur->wf[i]);
                        stay_on_WF = false;
                    }
                    break;
            }
        }
        if (stay_on_WF)     // we have reached the base case 
        {
            if (check_part_helper(cur, p, i))
                return;
            cur = get_workflow(wf_map, cur->base_h, cur->base);
        }
    }
}

void init_part(Part *p)
{
    p->x.min = p->m.min = p->a.min = p->s.min = MIN_RATING;
    p->x.max = p->m.max = p->a.max = p->s.max = MAX_RATING;
}

long long int calc(Workflow *wf_map[], Part p, int n_parts, char start[])
{
    long long int total = 0;
    int i;
    unsigned long int in_h = oaat(start, strlen(start), NUM_BITS);

    return total;
}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;
    static Workflow *wf_map[1 << NUM_BITS] = {NULL};
    Part p;

    line = read_line(WORD_LENGTH);
    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    parse_lines(lines, n, wf_map);
    init_part(&p);
    int total = calc(wf_map, p, "in");

    printf("total of accepted parts: %d\n", total);

    int i;
    for (i = 0; i < n; ++i)
    {
        free(lines[i]);
    }
    free(line);

    Workflow *wf, *tmp;
    for (i = 0; i < 1 << NUM_BITS; ++i)
    {
        wf = wf_map[i];
        while (wf != NULL)
        {
            tmp = wf->next;
            free(wf);
            wf = tmp;
        }
    }

    return 0;
}

