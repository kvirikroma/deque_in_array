#include <stdlib.h>
#include <string.h>

#include "deque.h"


static void** access_storage_by_index(deque* self, int32_t index)
{
    index = index % self->storage_size;
    if (index < 0)
    {
        index += self->storage_size;
    }
    return self->storage + (index);
}

static void** access_storage_by_address(deque* self, void** address)
{
    int32_t index = address - self->storage;
    return access_storage_by_index(self, index);
}

static void increase_storage_capacity(deque* self)
{
    self->storage_size <<= 1;  // *= 2
    void** old_storage_ptr = self->storage;
    self->storage = realloc(self->storage, self->storage_size * sizeof(void*));
    self->first_item += self->storage - old_storage_ptr;
    self->last_item += self->storage - old_storage_ptr;
    if (self->first_item > self->last_item)
    {
        uint32_t amount_to_move = (self->storage_size >> 1) - (self->first_item - self->storage);
        void** new_start = self->storage + self->storage_size - amount_to_move;
        memmove(
            new_start,
            self->first_item,
            amount_to_move * sizeof(void*)
        );
        self->first_item = new_start;
    }
}

static void deque_push_first_item(deque* self, void* value)
{
    self->first_item = self->storage;
    self->last_item = self->storage;
    *self->first_item = value;
}

static void shrink_storage(deque* self)
{
    if (
        (!self->storage_is_dynamic) ||
        (self->storage_size <= MINIMAL_SPACE) ||
        ((((self->storage_size - deque_get_count(self)) * 100) / self->storage_size) <= MAXIMUM_FREE_SPACE_PERCENT)
    )
    {
        return;
    }
    if (self->last_item && self->last_item)
    {
        if (self->first_item > self->last_item)
        {
            uint32_t amount_to_move = self->storage_size - (self->first_item - self->storage);
            void** new_place = self->storage + (self->storage_size >> 1) - amount_to_move;
            memmove(
                new_place,
                self->first_item,
                amount_to_move * sizeof(void*)
            );
            self->first_item = new_place;
        }
        else if ((self->last_item - self->storage) > (self->storage_size >> 1))
        {
            uint32_t deque_size = deque_get_count(self);
            memmove(
                self->storage,
                self->first_item,
                deque_size * sizeof(void*)
            );
            self->first_item = self->storage;
            self->last_item = self->storage + deque_size;
        }
    }
    self->storage_size >>= 1;  // /= 2
    void** old_storage_ptr = self->storage;
    self->storage = realloc(self->storage, self->storage_size * sizeof(void*));
    self->first_item += self->storage - old_storage_ptr;
    self->last_item += self->storage - old_storage_ptr;
}


void deque_init(deque* self, void* storage, uint32_t storage_size)
{
    if (storage && storage_size)
    {
        self->storage = storage;
        self->storage_size = storage_size;
        self->storage_is_dynamic = false;
    }
    else
    {
        self->storage = malloc(DEQUE_DEFAULT_INIT_SIZE * sizeof(void*));
        self->storage_size = DEQUE_DEFAULT_INIT_SIZE;
        self->storage_is_dynamic = true;
    }
    self->first_item = NULL;
    self->last_item = NULL;
}

void deque_delete(deque* self)
{
    if (self->storage_is_dynamic)
    {
        free(self->storage);
    }
    self->storage = NULL;
    self->last_item = NULL;
    self->first_item = NULL;
    self->storage_size = 0;
}

bool deque_push_left(deque* self, void* value)
{
    if (!self->last_item || !self->first_item)
    {
        deque_push_first_item(self, value);
        return true;
    }
    if (access_storage_by_address(self, self->first_item - 1) == self->last_item)
    {
        if (self->storage_is_dynamic)
        {
            increase_storage_capacity(self);
        }
        else
        {
            return false;
        }
    }
    self->first_item = access_storage_by_address(self, self->first_item - 1);
    *self->first_item = value;
    return true;
}

bool deque_push_right(deque* self, void* value)
{
    if (!self->last_item || !self->first_item)
    {
        deque_push_first_item(self, value);
        return true;
    }
    if (access_storage_by_address(self, self->last_item + 1) == self->first_item)
    {
        if (self->storage_is_dynamic)
        {
            increase_storage_capacity(self);
        }
        else
        {
            return false;
        }
    }
    self->last_item = access_storage_by_address(self, self->last_item + 1);
    *self->last_item = value;
    return true;
}

void* deque_pop_left(deque* self)
{
    if (!self->first_item || !self->last_item)
    {
        return 0;
    }
    void* result = *access_storage_by_address(self, self->first_item);
    if (self->last_item == self->first_item)
    {
        self->last_item = NULL;
        self->first_item = NULL;
    }
    else
    {
        self->first_item = access_storage_by_address(self, self->first_item + 1);
    }
    shrink_storage(self);
    return result;
}

void* deque_pop_right(deque* self)
{
    if (!self->first_item || !self->last_item)
    {
        return 0;
    }
    void* result = *access_storage_by_address(self, self->last_item);
    if (self->last_item == self->first_item)
    {
        self->last_item = NULL;
        self->first_item = NULL;
    }
    else
    {
        self->last_item = access_storage_by_address(self, self->last_item - 1);
    }
    shrink_storage(self);
    return result;
}

void* deque_get_by_index(deque* self, int32_t index)
{
    int32_t items_count = (int32_t)deque_get_count(self);
    index %= items_count;
    if (index < 0)
    {
        index += items_count;
    }
    return *access_storage_by_address(self, self->first_item + index);
}

void deque_set_by_index(deque* self, int32_t index, void* value)
{
    int32_t items_count = (int32_t)deque_get_count(self);
    index %= items_count;
    if (index < 0)
    {
        index += items_count;
    }
    *access_storage_by_address(self, self->first_item + index) = value;
}

bool deque_can_push(deque* self)
{
    if (!self->last_item || !self->first_item)
    {
        return true;
    }
    if ((access_storage_by_address(self, self->last_item + 1) == self->first_item) && (!self->storage_is_dynamic))
    {
        return false;
    }
    return true;
}

bool deque_can_pop(const deque* self)
{
    return (bool)(self->first_item && self->last_item);
}

uint32_t deque_get_count(const deque* self)
{
    if (!self->first_item || !self->last_item)
    {
        return 0;
    }
    int32_t result = self->last_item - self->first_item;
    if (result < 0)
    {
        result += self->storage_size;
    }
    result++;
    return result;
}

uint32_t deque_get_storage_size(const deque* self)
{
    return self->storage_size;
}

bool deque_can_realloc(const deque* self)
{
    return self->storage_is_dynamic;
}

void deque_iterate(deque* self, void(*item_receiver)(void* item, void* params), void* params)
{
    if (!self->first_item || !self->last_item)
    {
        return;
    }
    void** current_item = self->first_item - 1;
    do
    {
        current_item++;
        item_receiver(*access_storage_by_address(self, current_item), params);
    }
    while (access_storage_by_address(self, current_item) != self->last_item);
}

void deque_clear(deque* self)
{
    self->first_item = 0;
    self->last_item = 0;
    if (self->storage_is_dynamic)
    {
        self->storage_size = DEQUE_DEFAULT_INIT_SIZE;
        free(self->storage);
        self->storage = malloc(DEQUE_DEFAULT_INIT_SIZE * sizeof(void*));
    }
}
