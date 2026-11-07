#include "cstack.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACKS 10

struct node
{
    struct node* prev;
    unsigned int size;
    char data[];
};

typedef struct node* stack_t;

typedef struct stack_entry
{
    bool reserved;
    unsigned int size;
    stack_t stack_top;
} stack_entry_t;

static stack_entry_t g_table[MAX_STACKS];

hstack_t stack_new(void)
{
    for (hstack_t i = 0; i < MAX_STACKS; ++i)
    {
        if (!g_table[i].reserved)
        {
            g_table[i].reserved = true;
            g_table[i].size = 0;
            g_table[i].stack_top = NULL;
            return i;
        }
    }
    return -1;
}

void stack_free(const hstack_t hstack)
{
    if (!stack_valid_handler(hstack))
    {
        return;
    }
    while (g_table[hstack].stack_top != NULL)
    {
        stack_t stack = g_table[hstack].stack_top;
        g_table[hstack].stack_top = g_table[hstack].stack_top->prev;
        free(stack);
    }
    g_table[hstack].reserved = false;
    g_table[hstack].size = 0;
}

void stack_free_all(void)
{
    for (hstack_t hstack = 0; hstack < MAX_STACKS; ++hstack)
    {
        stack_free(hstack);
    }
}

int stack_valid_handler(const hstack_t hstack)
{
    if (hstack >= 0 && hstack < MAX_STACKS)
    {
        return (int)g_table[hstack].reserved;
    }
    return 0;
}

unsigned int stack_size(const hstack_t hstack)
{
    if (stack_valid_handler(hstack))
    {
        return g_table[hstack].size;
    }
    return 0;
}

void
stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    if (!stack_valid_handler(hstack) || !data_in || size == 0)
    {
        return;
    }
    stack_t stack = (stack_t)malloc(sizeof(struct node) + size);
    if (!stack)
    {
        return;
    }
    memcpy(stack->data, data_in, size);
    stack->prev = g_table[hstack].stack_top;
    stack->size = size;
    g_table[hstack].stack_top = stack;
    ++g_table[hstack].size;
}

unsigned int
stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    if (!stack_valid_handler(hstack) || !data_out || size == 0 ||
        g_table[hstack].size == 0)
    {
        return 0;
    }
    stack_t stack = g_table[hstack].stack_top;
    const unsigned int data_size = stack->size;
    if (data_size > size)
    {
        return 0;
    }
    memcpy(data_out, stack->data, data_size);
    g_table[hstack].stack_top = stack->prev;
    --g_table[hstack].size;
    free(stack);
    return data_size;
}

unsigned stack_top_size(const hstack_t hstack)
{
    if (stack_valid_handler(hstack) && g_table[hstack].stack_top)
    {
        return g_table[hstack].stack_top->size;
    }
    return 0;
}

hstack_t stack_get_all(void)
{
    hstack_t new = stack_new();
    if (new == -1)
    {
        return new;
    }
    for (hstack_t i = 0; i < MAX_STACKS; ++i)
    {
        if (i == new || !g_table[i].reserved)
        {
            continue;
        }
        const unsigned int old_size = stack_size(new);
        stack_push(new, &i, sizeof(hstack_t));
        if (stack_size(new) == old_size)
        {
            stack_free(new);
            return -1;
        }
    }
    return new;
}
