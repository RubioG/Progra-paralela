#include <assert.h>
#include <stdlib.h>

#include "array.h"

static array_t array_elements = NULL;
static size_t array_capacity = 0;
static size_t array_count = 0;

array_t array_create(size_t capacity)
{
	assert(capacity);
	array_capacity = capacity;
	array_count = 0;
	return array_elements = (void**)malloc( capacity * sizeof(void*) );
}

void array_destroy(array_t array)
{
	free(array);
}

int array_increase_capacity(array_t array)
{
	size_t new_capacity = 10 * array_capacity;
	array_t new_elements = (void**)realloc( array, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;

	array_capacity = new_capacity;
	array_elements = new_elements;

	return 0; // Success
}

int array_decrease_capacity(array_t array)
{
	size_t new_capacity = array_capacity / 10;
	if ( new_capacity < 10 )
		return 0;

	array_t new_elements = (void**)realloc( array, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;

	array_capacity = new_capacity;
	array_elements = new_elements;

	return 0; // Success
}

size_t array_get_count(const array_t array)
{
	(void)array;
	return array_count;
}

void* array_get_element(array_t array, size_t index)
{
	assert( index < array_get_count(array) );
	return array[index];
}

int array_append(array_t array, void* element)
{
	if ( array_count == array_capacity )
		if ( ! array_increase_capacity(array) )
			return -1;

	array[array_count++] = element;
	return 0; // Success
}

size_t array_find_first(const array_t array, const void* element, size_t start_pos)
{
	for ( size_t index = start_pos; index < array_count; ++index )
		if ( array[index] == element )
			return index;

	return array_not_found;
}

int array_remove_first(array_t array, const void* element, size_t start_pos)
{
	size_t index = array_find_first(array, element, start_pos);
	if ( index == array_not_found )
		return -1;

	for ( --array_count; index < array_count; ++index )
		array_elements[index] = array_elements[index + 1];
	if ( array_count == array_capacity / 10 )
		array_decrease_capacity(array);

	return 0; // Removed
}
