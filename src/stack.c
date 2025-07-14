#include "bootmem.h"
#include <stdio.h>

#include "stack.h"

stack_t *stack_new(size_t capacity)
{
    stack_t *stack = malloc(sizeof(stack_t));
    if (stack == NULL)
    {
        return NULL;
    }

    stack->count = 0;
    stack->capacity = capacity;
    stack->data = malloc(capacity * sizeof(void *));
    if (stack->data == NULL)
    {
        free(stack);
        return NULL;
    }

    return stack;
}

void stack_push(stack_t *stack, void *obj)
{
    if (stack->count == stack->capacity)
    {
        // Double stack capacity to avoid reallocing often
        stack->capacity *= 2;
        stack->data = realloc(stack->data, stack->capacity * sizeof(void *));
        if (stack->data == NULL)
        {
            // Unable to realloc, just exit :) get gud
            exit(1);
        }
    }

    stack->data[stack->count] = obj;
    stack->count++;

    return;
}

void *stack_pop(stack_t *stack)
{
    if (stack->count == 0)
    {
        return NULL;
    }

    stack->count--;
    return stack->data[stack->count];
}

void stack_free(stack_t *stack)
{
    if (stack == NULL)
    {
        return;
    }

    if (stack->data != NULL)
    {
        free(stack->data);
    }

    free(stack);
}

void stack_remove_nulls(stack_t *stack)
{
    size_t new_count = 0;

    // Iterate through the stack and count non-null elements
    for (int i = 0; i < stack->count; i++)
    {
        if (stack->data[i] != NULL)
        {
            stack->data[new_count++] = stack->data[i];
        }
    }

    stack->count = new_count;

    // Optionally zero out the remaining elements
    for (size_t i = new_count; i < stack->capacity; i++)
    {
        stack->data[i] = NULL;
    }
}
