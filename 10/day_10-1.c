#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LENGTH 16

char tiles[] = "|-LJ7F.S";

enum node_type
{
    VERT_PIPE,  // |
    HOR_PIPE,   // -
    NE_BEND,    // L
    NW_BEND,    // J
    SW_BEND,    // 7
    SE_BEND,    // F
    GROUND,     // .
    START       // S
};

typedef struct node
{
    int i, j;
    int type;
    struct node *next;
    struct node *prev;
} Node;

void print_node(Node *n)
{
    switch (n->type)
    {
        case VERT_PIPE:
            printf("type: | ");
            break;
        case HOR_PIPE:
            printf("type: - ");
            break;
        case NE_BEND:
            printf("type: L ");
            break;
        case NW_BEND:
            printf("type: J ");
            break;
        case SW_BEND:
            printf("type: 7 ");
            break;
        case SE_BEND:
            printf("type: F ");
            break;
        case GROUND:
            printf("type: . ");
            break;
        case START:
            printf("type: S ");
            break;
    }
    printf("at location i: %d, j: %d\n", n->i, n->j);
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

int get_node_type(char tile)
{
    return strchr(tiles, tile) - tiles;
}

void construct_graph(Node **graph, Node *start)
{
    Node *cur = start;
    Node *tmp;

    while (cur->next != start)
    {
        print_node(cur);
        switch (cur->type)
        {
            case VERT_PIPE:
                if (&graph[cur->i-1][cur->j] == cur->prev)
                {
                    cur->next = &graph[cur->i+1][cur->j];
                    tmp = cur;
                    cur = cur->next;
                    cur->prev = tmp;
                    break;
                }
                cur->next = &graph[cur->i-1][cur->j];
                tmp = cur;
                cur = cur->next;
                cur->prev = tmp;
                break;

            case HOR_PIPE:
                if (&graph[cur->i][cur->j-1] == cur->prev)
                {
                    cur->next = &graph[cur->i][cur->j+1];
                    tmp = cur;
                    cur = cur->next;
                    cur->prev = tmp;
                    break;
                }
                cur->next = &graph[cur->i][cur->j-1];
                tmp = cur;
                cur = cur->next;
                cur->prev = tmp;
                break;

            case NE_BEND:
                if (&graph[cur->i+1][cur->j] == cur->prev)
                {
                    cur->next = &graph[cur->i][cur->j+1];
                    tmp = cur;
                    cur = cur->next;
                    cur->prev = tmp;
                    break;
                }
                cur->next = &graph[cur->i+1][cur->j];
                tmp = cur;
                cur = cur->next;
                cur->prev = tmp;
                break;

            case NW_BEND:
                if (&graph[cur->i+1][cur->j] == cur->prev)
                {
                    cur->next = &graph[cur->i][cur->j-1];
                    tmp = cur;
                    cur = cur->next;
                    cur->prev = tmp;
                    break;
                }
                cur->next = &graph[cur->i+1][cur->j];
                tmp = cur;
                cur = cur->next;
                cur->prev = tmp;
                break;

            case SW_BEND:
                if (&graph[cur->i-1][cur->j] == cur->prev)
                {
                    cur->next = &graph[cur->i][cur->j-1];
                    tmp = cur;
                    cur = cur->next;
                    cur->prev = tmp;
                    break;
                }
                cur->next = &graph[cur->i-1][cur->j];
                tmp = cur;
                cur = cur->next;
                cur->prev = tmp;
                break;

            case SE_BEND:
                if (&graph[cur->i-1][cur->j] == cur->prev)
                {
                    cur->next = &graph[cur->i][cur->j+1];
                    tmp = cur;
                    cur = cur->next;
                    cur->prev = tmp;
                    break;
                }
                cur->next = &graph[cur->i-1][cur->j];
                tmp = cur;
                cur = cur->next;
                cur->prev = tmp;
                break;

            case GROUND:
                fprintf(stderr, "Something has gone wrong (case 'GROUND' reached)\n");
                break;
            case START:
                fprintf(stderr, "Something has gone wrong (case 'START' reached)\n");
                break;
            default:
                fprintf(stderr, "Something has gone wrong (default case reached)\n");
                break;
        }
    }
}

Node *parse_lines(char *lines[], int n)
{
    int i, j;
    int line_len = strlen(lines[0]);
    Node **graph;
    Node *start;

    graph = (Node **)malloc(sizeof(Node *) * n);

    for (i = 0; i < n; ++i)
    {
        graph[i] = (Node *)malloc(sizeof(Node) * line_len);

        for (j = 0; j < line_len; ++j)
        {
            graph[i][j].i = i;
            graph[i][j].j = j;
            graph[i][j].type = get_node_type(lines[i][j]);

            if (graph[i][j].type == START)
            {
                start = &graph[i][j];
                start->type = get_node_type('J');   // HARD CODE THE START NODE TYPE (I dont care)
            }
        }
    }
    construct_graph(graph, start);

    return start;
}

int calc_steps(Node *start)
{
    int n = 0;
    Node *n0 = start->next;
    Node *n1 = start->prev;
    while (n0 != n1)
    {
        n0 = n0->next;
        n1 = n1->prev;
        ++n;
    }

    return n;
}

int main()
{
    char *line;
    char *lines[1024];
    int n = 0;

    line = read_line(WORD_LENGTH);

    while (*line) // line != NULL
    {
        lines[n] = line;
        line = read_line(WORD_LENGTH);
        ++n;
    }

    Node *start = parse_lines(lines, n);
    int total = calc_steps(start);

    printf("steps: %d\n", total);

    return 0;
}
