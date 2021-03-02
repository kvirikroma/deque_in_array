#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "deque.h"


int main()
{
    deque data;
    deque_init(&data, NULL, 0);
    for (int i = 0; i < 28; i++)
    {
        deque_push_right(&data, (void*)(uint64_t)i);
    }
    deque_set_by_index(&data, -234233, (void*)987765);
    for (int i = 0; i < deque_get_count(&data); i++)
    {
        printf("%d\n", (int)(uint64_t)deque_get_by_index(&data, i));
    }
    for (int i = 0; i < 28; i++)
    {
        deque_pop_right(&data);
    }
    deque_delete(&data);
    return 0;
}
