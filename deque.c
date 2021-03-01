#include <stdlib.h>

#include "deque.h"


void deque_init(deque* self, void* storage, uint32_t storage_size)
{
    if (storage)
    {
        self->storage = storage;
        self->storage_size = storage_size;
        self->dynamic_storage = false;
    }
    else
    {
        self->storage = malloc(DEQUE_DEFAULT_INIT_SIZE);
        self->storage_size = DEQUE_DEFAULT_INIT_SIZE;
        self->dynamic_storage = true;
    }
    self->first_item = NULL;
    self->last_item = NULL;
}

void deque_delete(deque* self)
{
    if (self->dynamic_storage)
    {
        free(self->storage);
    }
    self->storage = NULL;
    self->last_item = NULL;
    self->first_item = NULL;
    self->storage_size = 0;
}

bool deque_push_left(deque* self, void* item);

bool deque_push_right(deque* self, void* item);

void* deque_pop_left(deque* self);

void* deque_pop_right(deque* self);

void* deque_get_by_index(deque* self, uint32_t index);

bool deque_set_by_index(deque* self, uint32_t index, void* value);

bool deque_can_push(deque* self);

bool deque_can_pop(deque* self);

uint32_t deque_get_size(deque* self);

uint32_t deque_get_storage_size(deque* self)
{
    return self->storage_size;
}

bool deque_can_realloc(deque* self)
{
    return self->dynamic_storage;
}

void deque_iterate(deque* self, void(*item_receiver)(void* item, void* params), void* params);
