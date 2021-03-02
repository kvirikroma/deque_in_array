#ifndef DEQUE_H
#define DEQUE_H

#include <stdbool.h>
#include <stdint.h>

#define DEQUE_DEFAULT_INIT_SIZE 16  // items
#define MAXIMUM_FREE_SPACE_PERCENT 75
#define MINIMAL_SPACE DEQUE_DEFAULT_INIT_SIZE


// This structure uses an Opizdulation principle.
// This means that you should be punched in a face (by the author, probably) if you access its fields directly outside the 'deque.c'.
// Use fucking functions for it!
typedef struct
{
    void** storage;
    void** first_item;
    void** last_item;
    uint32_t storage_size;  // items
    bool storage_is_dynamic;
}
deque;


/// This function initializes the deque structure.
/// @param storage is needed for ability to use statically allocated memory instead of malloc.
/// It should be NULL if you want to use malloc instead (reallocates automatically).
/// @param storage_size is only needed for those cases when @param storage is not NULL; otherwise it should also be NULL.
void deque_init(deque* self, void* storage, uint32_t storage_size);

/// This function deletes the deque structure and frees allocated memory if there is some.
void deque_delete(deque* self);

/// Push the void* value to the left of the deque.
/// @param item - value to push.
/// @returns true if the operation is successful
bool deque_push_left(deque* self, void* value);

/// Push the void* value to the right of the deque.
/// @param item - value to push.
/// @returns true if the operation is successful
bool deque_push_right(deque* self, void* value);

/// Pop the void* value from the left of the deque.
/// @returns value that have been popped.
void* deque_pop_left(deque* self);

/// Pop the void* value from the right of the deque.
/// @returns value that have been popped.
void* deque_pop_right(deque* self);

/// Get the item from deque by index
/// Returns 0 if the index is not present in the deque
void* deque_get_by_index(deque* self, int32_t index);

/// Change the item in deque by its index
/// @returns true if the operation is successful
void deque_set_by_index(deque* self, int32_t index, void* value);

/// Checks if the value can be pushed into the deque.
/// @returns true if value can be pushed.
bool deque_can_push(deque* self);

/// Checks if the value can be popped from the deque.
/// @returns true if value can be popped.
bool deque_can_pop(const deque* self);

/// @returns count of items in the deque
uint32_t deque_get_count(const deque* self);

/// @returns real size of storage
uint32_t deque_get_storage_size(const deque* self);

/// @returns true if the internal storage was allocated in heap
bool deque_can_realloc(const deque* self);

/// Applies the @param item_receiver function to each item in the deque.
/// @param params is needed to pass some additional parameters to the @param item_receiver.
void deque_iterate(deque* self, void(*item_receiver)(void* item, void* params), void* params);

/// Clears the deque (in O(1))
void deque_clear(deque* self);


#endif
