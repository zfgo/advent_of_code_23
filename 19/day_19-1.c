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
    char xmas[8];
    int val[8];
    Cond cond[8];
    int i;

    // rules: goto this rule:
    //  'A' for accepted
    //  'R' for rejected
    //  <rule->name> if another rule is needed
    char rules[8][8];

    // hashes of the other rules
    //  <rule_name> : hash
    //  'A' : ACCEPTED (2049)
    //  'R' : REJECTED (2050)
    //  doesn't exist : DOES_NOT_EXIST (2051)
    unsigned long int rules_h[8];
    unsigned long int hash; // it might be convenient to store the workflows in 
                            // a hash table based on the name of the rule
    char base[8];    // default (or `base') rule
    unsigned long int base_h;     // hash of the default rule
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
    for (int i = 0; i < r->i; ++i)
    {
        if (i == r->i - 1)
        {
            printf("%c%c%d: %s", r->xmas[i], (r->cond[i] == LT) ? '<' : '>', 
                    r->val[i], r->rules[i]);
            break;
        }
        printf("%c%c%d: %s, ", r->xmas[i], (r->cond[i] == LT) ? '<' : '>', 
                r->val[i], r->rules[i]);
    }
    printf(") hash: %lu\n", r->hash);
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
    int i, j;
    char name[8];
    char rules[64];
    Rule *r;

    for (i = 0; lines[i][0] != '{'; ++i)
    {
        r = (Rule *)malloc(sizeof(Rule));
        r->i = 0;

        // scan in the line into name and rules bufs
        sscanf(lines[i], "%[a-z]{%[a-zA-Z0-9:<>,]}", name, rules);
        strcpy(r->name, name);  // set the rule's name
        r->hash = oaat(name, strlen(name), NUM_BITS); // and get the rule's hash

        char *tok = strtok(rules, ","); // tokenize the rules based on ","

        while (tok != NULL)
        {
            if (strlen(tok) < 5) // this is the last instruction 
            {                    // (minimum non-last instruction size is 5)
                strcpy(r->base, tok);
                r->base_h = oaat(tok, strlen(tok), NUM_BITS);
            }
            else
            {
                char ltgt;

                sscanf(tok, "%c%c%d:%[a-z]", &r->xmas[r->i], &ltgt, 
                        &r->val[r->i], r->rules[r->i]);
                r->cond[r->i] = (ltgt == '<') ? LT : GT;
                r->rules_h[r->i] = oaat(r->rules[r->i], strlen(r->rules[r->i]), NUM_BITS);
                r->i++;
            }
            tok = strtok(NULL, ",");
        }
        r->next = rule_map[r->hash];
        rule_map[r->hash] = r;
    }
                
    for (j = 0 /* `i` is unchanged */ ; i < n; ++i, ++j)
    {
        sscanf(lines[i], "{x=%d,m=%d,a=%d,s=%d}", &parts[j].x, &parts[j].m, 
                &parts[j].a, &parts[j].s);
    }

    *n_parts = j;
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

Rule *get_rule(Rule *rule_map[], unsigned long int hash, char name[])
{
    Rule *r;
    r = rule_map[hash];

    while (r != NULL && strcmp(r->name, name) != 0)
    {
        r = r->next;
    }

    return r;
}

bool check_part_helper(Rule *cur, Part *p, int i)
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

    switch (cur->rules_h[i])
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

void check_part(Rule *rule_map[], Part *p, unsigned long int in_h)
{
    int i;
    Rule *cur;
    bool stay_on_WF;

    cur = rule_map[in_h];
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
                        cur = get_rule(rule_map, cur->rules_h[i], cur->rules[i]);
                        stay_on_WF = false;
                    }
                    break;
                case 'm':
                    if (check_rule(cur->cond[i], cur->val[i], p->m))
                    {
                        if (check_part_helper(cur, p, i))
                            return;
                        cur = get_rule(rule_map, cur->rules_h[i], cur->rules[i]);
                        stay_on_WF = false;
                    }
                    break;
                case 'a':
                    if (check_rule(cur->cond[i], cur->val[i], p->a))
                    {
                        if (check_part_helper(cur, p, i))
                            return;
                        cur = get_rule(rule_map, cur->rules_h[i], cur->rules[i]);
                        stay_on_WF = false;
                    }
                    break;
                case 's':
                    if (check_rule(cur->cond[i], cur->val[i], p->s))
                    {
                        if (check_part_helper(cur, p, i))
                            return;
                        cur = get_rule(rule_map, cur->rules_h[i], cur->rules[i]);
                        stay_on_WF = false;
                    }
                    break;
            }
            if (stay_on_WF)
            {
                // we have reached the base case 
                if (check_part_helper(cur, p, i))
                {
                    return;
                }
                cur = get_rule(rule_map, cur->base_h, cur->base);
            }
        }
    }
}

int calc(Rule *rule_map[], Part parts[], int n_parts)
{
    int total = 0;
    int i;
    unsigned long int in_h = oaat("in", strlen("in"), NUM_BITS);

    for (i = 0; i < n_parts; ++i)
    {
        check_part(rule_map, &parts[i], in_h);
        if (parts[i].accepted)
        {
            total += (parts[i].x + parts[i].m + parts[i].a + parts[i].s);
        }
    }
    return total;
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
    int total = calc(rule_map, parts, n_parts);

    printf("total of accepted parts: %d\n", total);

    return 0;
}
