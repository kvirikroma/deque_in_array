#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "deque.h"


int main()
{
    void* storage[16];
    deque data;
    deque_init(&data, storage, sizeof(storage) / sizeof(void*));
    for (int i = 0; i < 28; i++)
    {
        printf("%d\n", deque_push_left(&data, (void*)(uint64_t)i));
    }
    for (int i = 0; i < 8; i++)
    {
        printf("%d\n", (int)(uint64_t)deque_pop_left(&data));
        fflush(stdout);
    }
    for (int i = 0; i < 28; i++)
    {
        printf("%d\n", deque_push_right(&data, (void*)(uint64_t)i));
    }
    for (int i = 0; i < 16; i++)
    {
        printf("%d\n", (int)(uint64_t)deque_pop_right(&data));
        fflush(stdout);
    }
    printf("%d\n", deque_get_count(&data));
    deque_delete(&data);
    return 0;
}
