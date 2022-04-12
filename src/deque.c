#include <stdlib.h>
#include <string.h>

#include "deque.h"


static deque_item* access_storage_by_index(deque* self, int32_t index)
{
    index %= self->storage_size;
    index += self->storage_size * (index < 0);
    return (deque_item*)(self->item_size * index + self->storage);
}

static int8_t* access_storage_by_address(deque* self, int8_t* address)
{
    int32_t index = (address - self->storage) / self->item_size;
    return (int8_t*)(access_storage_by_index(self, index));
}

static void increase_storage_capacity(deque* self)
{
    self->storage_size <<= 1;  // *= 2
    int8_t* old_storage_ptr = self->storage;
    self->storage = realloc(self->storage, self->storage_size * self->item_size);
    self->first_item += self->storage - old_storage_ptr;
    self->last_item += self->storage - old_storage_ptr;
    if (self->first_item > self->last_item)
    {
        uint32_t bytes_to_move = (self->storage_size >> 1) * self->item_size - (self->first_item - self->storage);
        int8_t* new_start = self->storage + (self->item_size * self->storage_size) - bytes_to_move;
        memmove(
            new_start,
            self->first_item,
            bytes_to_move
        );
        self->first_item = new_start;
    }
}

static void deque_push_first_item(deque* self, deque_item* value)
{
    self->first_item = self->storage;
    self->last_item = self->storage;
    memcpy((deque_item*)self->first_item, value, self->item_size);
}

static void shrink_storage(deque* self)
{
    if (
        (!self->storage_is_dynamic) ||
        (self->storage_size <= MINIMAL_SPACE) ||
        (
            (((self->storage_size - deque_get_count(self)) * 100) / self->storage_size) <= 
            MAXIMUM_FREE_SPACE_PERCENT
        )
    )
    {
        return;
    }
    if (self->first_item && self->last_item)
    {
        if (self->first_item > self->last_item)
        {
            uint32_t bytes_to_move =
                self->storage_size * self->item_size - (self->first_item - self->storage);
            int8_t* new_place =
                self->storage + ((self->storage_size >> 1) * self->item_size) - bytes_to_move;
            memmove(
                new_place,
                self->first_item,
                bytes_to_move
            );
            self->first_item = new_place;
        }
        else if ((self->last_item - self->storage) > ((self->storage_size >> 1) * self->item_size))
        {
            // This part is a bit shitty 'cause I could just wrap right items around to start of the
            // self->storage when self->first_item placed in the left half of the storage,
            // but I don't care.
            // Maybe will improve this in future
            uint32_t deque_size = deque_get_count(self);
            memmove(
                self->storage,
                self->first_item,
                deque_size * self->item_size
            );
            self->first_item = self->storage;
            self->last_item = self->storage + (deque_size * self->item_size);
        }
    }
    self->storage_size >>= 1;  // /= 2
    int8_t* old_storage_ptr = self->storage;
    self->storage = realloc(self->storage, self->storage_size * self->item_size);
    self->first_item += self->storage - old_storage_ptr;
    self->last_item += self->storage - old_storage_ptr;
}


void deque_init(deque* self, void* storage, uint32_t storage_size, uint32_t item_size)
{
    if (item_size == 0)
    {
        self->storage = NULL;
        self->last_item = NULL;
        self->first_item = NULL;
        self->storage_size = 0;
        self->item_size = 0;
        return;
    }
    self->item_size = item_size;
    if (storage && storage_size)
    {
        self->storage = storage;
        self->storage_size = storage_size;
        self->storage_is_dynamic = false;
    }
    else
    {
        self->storage = malloc(DEQUE_DEFAULT_INIT_SIZE * self->item_size);
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
    self->item_size = 0;
}

bool deque_push_left(deque* self, deque_item* value)
{
    if (!self->last_item || !self->first_item)
    {
        deque_push_first_item(self, value);
        return true;
    }
    if (access_storage_by_address(self, self->first_item - self->item_size) == self->last_item)
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
    self->first_item = access_storage_by_address(self, self->first_item - self->item_size);
    memcpy((deque_item*)self->first_item, value, self->item_size);
    return true;
}

bool deque_push_right(deque* self, deque_item* value)
{
    if (!self->last_item || !self->first_item)
    {
        deque_push_first_item(self, value);
        return true;
    }
    if (access_storage_by_address(self, self->last_item + self->item_size) == self->first_item)
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
    self->last_item = access_storage_by_address(self, self->last_item + self->item_size);
    memcpy((deque_item*)self->last_item, value, self->item_size);
    return true;
}

bool deque_pop_left(deque* self, deque_item* destination)
{
    if (!self->first_item || !self->last_item)
    {
        return false;
    }
    int8_t* result = access_storage_by_address(self, self->first_item);
    if (self->last_item == self->first_item)
    {
        self->last_item = NULL;
        self->first_item = NULL;
    }
    else
    {
        self->first_item = access_storage_by_address(self, self->first_item + self->item_size);
    }
    shrink_storage(self);
    if (destination)
    {
        memcpy(destination, result, self->item_size);
    }
    return true;
}

bool deque_pop_right(deque* self, deque_item* destination)
{
    if (!self->first_item || !self->last_item)
    {
        return false;
    }
    int8_t* result = access_storage_by_address(self, self->last_item);
    if (self->last_item == self->first_item)
    {
        self->last_item = NULL;
        self->first_item = NULL;
    }
    else
    {
        self->last_item = access_storage_by_address(self, self->last_item - self->item_size);
    }
    shrink_storage(self);
    if (destination)
    {
        memcpy(destination, result, self->item_size);
    }
    return true;
}

void deque_delete_from_left(deque* self, uint32_t count)
{
    if (deque_get_count(self) <= count)
    {
        deque_clear(self);
    }
    else
    {
        self->first_item = access_storage_by_address(
            self, self->first_item + (self->item_size * count)
            );
        shrink_storage(self);
    }
}

void deque_delete_from_right(deque* self, uint32_t count)
{
    if (deque_get_count(self) <= count)
    {
        deque_clear(self);
    }
    else
    {
        self->last_item = access_storage_by_address(
            self, self->last_item - (self->item_size * count)
            );
        shrink_storage(self);
    }
}

static bool normalize_index(deque* self, int32_t* index)
{
    int32_t items_count = (int32_t)deque_get_count(self);
    if (items_count == 0)
    {
        return false;
    }
    *index %= items_count;
    *index += items_count * (*index < 0);
    return true;
}

bool deque_copy_by_index(deque* self, int32_t index, deque_item* destination)
{
    if (!normalize_index(self, &index))
    {
        return false;
    }
    index += self->first_item - self->storage;
    memcpy(destination, access_storage_by_index(self, index), self->item_size);
    return true;
}

deque_item* deque_get_by_index(deque* self, int32_t index)
{
    if (!normalize_index(self, &index))
    {
        return false;
    }
    index += self->first_item - self->storage;
    return access_storage_by_index(self, index);
}

bool deque_set_by_index(deque* self, int32_t index, deque_item* source)
{
    if (!normalize_index(self, &index))
    {
        return false;
    }
    index += self->first_item - self->storage;
    memcpy(access_storage_by_index(self, index), source, self->item_size);
    return true;
}

bool deque_can_push(deque* self)
{
    if (!self->last_item || !self->first_item)
    {
        return true;
    }
    if (
        (access_storage_by_address(self, self->last_item + self->item_size) == self->first_item) &&
        (!self->storage_is_dynamic)
    )
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
    result /= self->item_size;
    result += self->storage_size * (result < 0);
    result++;
    return result;
}

bool deque_is_empty(deque* self)
{
	return !self->first_item || !self->last_item;
}

uint32_t deque_get_storage_size(const deque* self)
{
    return self->storage_size;
}

bool deque_can_realloc(const deque* self)
{
    return self->storage_is_dynamic;
}

uint32_t deque_get_item_size(const deque* self)
{
    return self->item_size;
}

void deque_iterate(deque* self, void(*item_receiver)(deque_item* item, void* params), void* params)
{
    if (!self->first_item || !self->last_item)
    {
        return;
    }
    int32_t current_item = -1;
    do
    {
        current_item++;
        item_receiver(access_storage_by_index(self, current_item), params);
    }
    while (access_storage_by_index(self, current_item) != (deque_item*)self->last_item);
}

void deque_clear(deque* self)
{
    self->first_item = 0;
    self->last_item = 0;
    if (self->storage_is_dynamic)
    {
        self->storage_size = DEQUE_DEFAULT_INIT_SIZE;
        free(self->storage);
        self->storage = malloc(DEQUE_DEFAULT_INIT_SIZE * self->item_size);
    }
}
