#ifndef DEQUE_H
#define DEQUE_H

#include <stdbool.h>
#include <stdint.h>

#define DEQUE_DEFAULT_INIT_SIZE 16  // items
#define MAXIMUM_FREE_SPACE_PERCENT 75  // must be more than 50 and less than 100 (not equal!)
#define MINIMAL_SPACE DEQUE_DEFAULT_INIT_SIZE


typedef struct
{
    char first_byte;
    char other_bytes[];
}
deque_item;

typedef union __attribute__((__packed__))
{
    uint8_t _;
}
byte;  // only size matters for this type, not the sign

// This structure uses an Opizdulation principle.
// This means that you should be punched in a face (by the author, probably) if you access its fields directly outside the 'deque.c'.
// Use fucking functions for it!
typedef struct
{
    byte* storage;
    byte* first_item;
    byte* last_item;
    uint32_t storage_size;  // items count
    uint32_t item_size;
    bool storage_is_dynamic;
}
deque;


/// This function initializes the deque structure.
/// @param storage is needed for ability to use statically allocated memory instead of malloc.
/// It should be NULL if you want to use malloc instead (reallocates automatically).
/// @param storage_size indicates max count of items and is only needed for those cases
/// when storage is not NULL; otherwise it should also be NULL.
void deque_init(deque* self, void* storage, uint32_t storage_size, uint32_t item_size);

/// This function deletes the deque structure and frees memory that was allocated (if it was).
void deque_delete(deque* self);

/// Push the void* value to the left of the deque.
/// @param value value to push.
/// @returns true if the operation is successful
bool deque_push_left(deque* self, deque_item* value);

/// Push the value to the right of the deque.
/// @param value value to push.
/// @returns true if the operation is successful
bool deque_push_right(deque* self, deque_item* value);

/// Pop the value from the left of the deque to the destination.
/// This is ok when destination is NULL
/// @returns true if the operation is successful
bool deque_pop_left(deque* self, deque_item* destination);

/// Pop the value from the right of the deque to the destination.
/// This is ok when destination is NULL
/// @returns true if the operation is successful
bool deque_pop_right(deque* self, deque_item* destination);

/// Get the item pointer in deque by its index
/// @returns pointer to deque item
deque_item* deque_item_ptr_by_index(deque* self, int32_t index);

/// Checks if some value can be pushed into the deque.
/// @returns true if value can be pushed.
bool deque_can_push(deque* self);

/// Checks if some value can be popped from the deque.
/// @returns true if value can be popped.
bool deque_can_pop(const deque* self);

/// @returns count of items in the deque (in O(1))
uint32_t deque_get_count(const deque* self);

/// @returns real size of storage
uint32_t deque_get_storage_size(const deque* self);

/// @returns true if the internal storage was allocated in heap
bool deque_can_realloc(const deque* self);

/// @returns size of each item
uint32_t deque_get_item_size(const deque* self);

/// Applies the item_receiver function to each item in the deque.
/// @param params is needed to pass some additional parameters to the item_receiver.
void deque_iterate(deque* self, void(*item_receiver)(deque_item* item, void* params), void* params);

/// Clears the deque (in O(1))
void deque_clear(deque* self);


#endif
