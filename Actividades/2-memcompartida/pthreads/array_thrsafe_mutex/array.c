#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "array.h"

//static array_t array_elements = NULL;
//static size_t array_capacity = 0;
//static size_t array_count = 0;

array_t* array_create(size_t capacity)
{
	assert(capacity);
	array_t* array = (array_t*)malloc(sizeof(array_t));
	array->capacity = capacity;
	array->count = 0;
	array->elements = (void**)malloc( capacity * sizeof(void*) );
	if (pthread_mutex_init(&(array->lock), NULL) != 0)
	{
		 printf("\n mutex init failed\n");
	}
	return array;
}

void array_destroy(array_t* array)
{
	free(array->elements);
	free(array);
}

int array_increase_capacity(array_t* array)
{
	pthread_mutex_lock(&(array->lock));
	size_t new_capacity = 10 * array->capacity;
	void** new_elements = (void**)realloc( array->elements, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;

	array->capacity = new_capacity;
	array->elements = new_elements;
	pthread_mutex_unlock(&(array->lock));

	return 0; // Success
}

int array_decrease_capacity(array_t* array)
{
	pthread_mutex_lock(&(array->lock));
	size_t new_capacity = array->capacity / 10;
	if ( new_capacity < 10 )
		return 0;

	void** new_elements = (void**)realloc( array->elements, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;

	array->capacity = new_capacity;
	array->elements = new_elements;
	pthread_mutex_unlock(&(array->lock));
	return 0; // Success
}

size_t array_get_count(const array_t* array)
{
	//printf("\nContador actual %zu\n", array->count);
	return array->count;
}

void* array_get_element(array_t* array, size_t index)
{
	assert( index < array_get_count(array) );
	//printf(" Elemento %zu\n", (size_t)array->elements[index]);
	return array->elements[index];
}

int array_append(array_t* array, void* element)
{
	pthread_mutex_lock(&(array->lock));
	if ( array->count == array->capacity )
		if ( ! array_increase_capacity(array) )
			return -1;

	//printf("Count actual %zu\n, Capacidad %zu\n", array->count, array->capacity);
	array->elements[array->count] = element;
	array->count += 1;
	pthread_mutex_unlock(&(array->lock));
	//printf("\nSe inserto el elemento %zu, se supone que debe de ser %zu", (size_t)array->elements[array->count-1], (size_t)element);
	return 0; // Success
}

size_t array_find_first(array_t* array, const void* element, size_t start_pos)
{
	pthread_mutex_lock(&(array->lock));
	for ( size_t index = start_pos; index < array->count; ++index )
		if ( array->elements[index] == element )
			return index;

	pthread_mutex_unlock(&(array->lock));
	return (size_t)-1;
}

int array_remove_first(array_t* array, const void* element, size_t start_pos)
{
	pthread_mutex_lock(&(array->lock));
	size_t index = array_find_first(array, element, start_pos);
	if ( index == (size_t)-1 )
		return -1;

	for ( --array->count; index < array->count; ++index )
		array->elements[index] = array->elements[index + 1];
	if ( array->count == array->capacity / 10 )
		array_decrease_capacity(array);
	pthread_mutex_unlock(&(array->lock));
	return 0; // Removed
}
