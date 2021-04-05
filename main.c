#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "deque.h"


void print_int_item(deque_item* item, void* params)
{
    int int_to_print[6] = {0};
    uint32_t* size_param = params;
    memcpy(int_to_print, item, *size_param);
    printf("%d\n", *int_to_print);
}

int main()
{
    deque data;
    deque_init(&data, NULL, 0, 23);
    int i[6] = {0};
    for (i[0] = 1000; i[0] < 1028; i[0]++)
    {
        deque_push_right(&data, (deque_item*)i);
    }
    union{float f_value; char str_value[24];} value;
    value.f_value = 987.765;
    deque_set_by_index(&data, -234233, (deque_item*)&value);
    int items_count = deque_get_count(&data);
    for (i[0] = 0; i[0] < deque_get_count(&data); i[0]++)
    {
        int value_to_print[6] = {0};
        deque_get_by_index(&data, i[0], (deque_item*)value_to_print);
        value_to_print[0]++;
        deque_set_by_index(&data, i[0], (deque_item*)value_to_print);
        value_to_print[0]--;
        printf("%d\n", value_to_print[0]);
    }
    int item_size = deque_get_item_size(&data);
    deque_iterate(&data, print_int_item, &item_size);
    for (i[0] = 0; i[0] < items_count; i[0]++)
    {
        deque_pop_right(&data, NULL);
    }
    deque_clear(&data);
    deque_delete(&data);
    return 0;
}
