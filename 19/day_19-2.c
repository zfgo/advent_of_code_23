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

// define the map as a global variable
static Workflow *wf_map[1 << NUM_BITS] = {NULL};

// define the ranges
bool x[4001];
bool m[4001];
bool a[4001];
bool s[4001];

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

/*
void print_part(Part *p)
{
    printf("x: %d, m: %d, a: %d, s: %d, acc: %s\n", p->x, p->m, p->a, p->s, 
            p->accepted ? "true" : "false");
}
*/

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

void parse_lines(char *lines[])
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
        //print_rule(wf);
        //printf("wfs parsed: %d\n", i+1);
    }
}

Workflow *get_workflow(unsigned long int hash, char name[])
{
    Workflow *wf;
    if (hash > (1 << NUM_BITS))
    {
        fprintf(stderr, "ERROR: about to get a rule for hash %lu\n", hash);
    }
    wf = wf_map[hash];

    while (wf != NULL && strcmp(wf->name, name) != 0)
    {
        wf = wf->next;
    }

    return wf;
}

typedef enum
{
    FALSE = 0,   // the range can NOT be trimmed to fit the condition
    LTC,     // the range is already less than
    GTC,     // the range is already greater than
    LT_NT,  // the range can be less than, but it needs trimming
    GT_NT   // the range can be greater than, but it needs trimming
} Range_Cond;

Range_Cond check_part(Part p, char xmas, Cond c, int val)
{
    Range r;

    switch (xmas)
    {
    case 'x':
        r = p.x;
        break;
    case 'm':
        r = p.m;
        break;
    case 'a':
        r = p.a;
        break;
    case 's':
        r = p.s;
        break;
    }

    switch (c)
    {
    case LT:
        if (r.max < val)
            return LTC;
        else if (r.min < val)
            return LT_NT;
        break;
    case GT:
        if (r.min > val)
            return GTC;
        else if (r.max > val)
            return GT_NT;
        break;
    default:
        return FALSE;
    }
    return FALSE;
}

void init_part(Part *p)
{
    p->x.min = p->m.min = p->a.min = p->s.min = MIN_RATING;
    p->x.max = p->m.max = p->a.max = p->s.max = MAX_RATING;
}

long long int calc_part_sum(Part p)
{
    return (p.x.max - p.x.min + 1) *
        (p.m.max - p.m.min + 1) *
        (p.a.max - p.a.min + 1) *
        (p.s.max - p.s.min + 1);
}

typedef enum min_max 
{
    MIN,
    MAX
} Min_Max;

void trim_range(Range *r, Min_Max min_max, int val)
{
    switch (min_max)
    {
    case MIN:
        r->min = val;
        return;
    case MAX:
        r->max = val;
        return;
    }
}

void trim_part(Part *p, char xmas, Min_Max min_max, int val)
{
    switch (xmas)
    {
    case 'x':
        trim_range(&p->x, min_max, val);
        return;
    case 'm':
        trim_range(&p->m, min_max, val);
        return;
    case 'a':
        trim_range(&p->a, min_max, val);
        return;
    case 's':
        trim_range(&p->s, min_max, val);
        return;
    }
}

void copy_range(Range *dest, Range src)
{
    dest->min = src.min;
    dest->max = src.max;
}

void copy_part(Part *dest, Part src)
{
    copy_range(&dest->x, src.x);
    copy_range(&dest->m, src.m);
    copy_range(&dest->a, src.a);
    copy_range(&dest->s, src.s);
}

long long int calc(Part p, char wf_name[], int wf_i)
{
    unsigned long int wf_h = oaat(wf_name, strlen(wf_name), NUM_BITS);
    printf("CALL TO calc: wf_h hash: %lu, wf name: %s, wf_i: %d\n", 
            wf_h, wf_name, wf_i);

    if (wf_h == ACCEPTED)
    {
        printf("Accepted\n");
        return calc_part_sum(p);
    }
    else if (wf_h == REJECTED)
    {
        printf("Rejected\n");
        return 0LL;
    }

    Workflow *wf = get_workflow(wf_h, wf_name);


    if (wf_i == wf->i)
    {
        if (wf->base_h == ACCEPTED)
        {
            printf("Accepted\n");
            return calc_part_sum(p);
        }
        else if (wf->base_h == REJECTED)
        {
            printf("Rejected\n");
            return 0LL;
        }
        else 
            return calc(p, wf->base, 0);
    }

    Part lp, rp;

    copy_part(&lp, p);
    copy_part(&rp, p);

    switch (check_part(p, wf->xmas[wf_i], wf->cond[wf_i], wf->val[wf_i]))
    {
    case LTC:
        return calc(lp, wf->wf[wf_i], 0) + calc(rp, wf->name, wf_i+1);

    case LT_NT:
        trim_part(&lp, wf->xmas[wf_i], MAX, wf->val[wf_i]-1);
        trim_part(&rp, wf->xmas[wf_i], MAX, wf->val[wf_i]);
        return calc(lp, wf->wf[wf_i], 0) + calc(rp, wf->name, wf_i+1);

    case GTC:
        return calc(lp, wf->wf[wf_i], 0) + calc(rp, wf->name, wf_i+1);

    case GT_NT:
        trim_part(&lp, wf->xmas[wf_i], MIN, wf->val[wf_i]);
        trim_part(&rp, wf->xmas[wf_i], MIN, wf->val[wf_i]+1);
        return calc(lp, wf->wf[wf_i], 0) + calc(rp, wf->name, wf_i+1);

    case FALSE:
        printf("False path\n");
        return 0LL; // it is impossible to go down this path
    }
    // TODO: check the condition. If the condition is already true, no need 
    // to trim the range. If the condition is false, and we can trim the range,
    // then trim the range, and do the recursive call for the left and the 
    // right ranges. Otherwise, ???

    return 0LL;
}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;
    Part p;

    line = read_line(WORD_LENGTH);
    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    parse_lines(lines);
    init_part(&p);
    long long int total = calc(p, "in", 0);

    printf("total of accepted parts: %lld\n", total);

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

