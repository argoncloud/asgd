#ifndef _ASGD_DATA_H_
#define _ASGD_DATA_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// variable size buffer
typedef struct _asgd_data_buffer asgd_data_buffer_t;
struct _asgd_data_buffer
{
	float *buffer;
};

void asgd_data_buffer_init(asgd_data_buffer_t *buffer);

float *asgd_data_buffer_get(asgd_data_buffer_t *buffer, size_t size);

void asgd_data_buffer_destr(asgd_data_buffer_t *buffer);

// X
typedef struct _asgd_data_X asgd_data_X_t;
struct _asgd_data_X
{
	bool (*next_block)(asgd_data_X_t *state, float **data, size_t *rows);
};

typedef struct _asgd_data_X_memory asgd_data_X_memory_t;
struct _asgd_data_X_memory
{
	asgd_data_X_t data;

	float *items;
	size_t n_points;
	size_t n_feats;
	size_t points_left;
	size_t batch_size;
};

void asgd_data_X_memory_init(
		asgd_data_X_memory_t *data,
		float *items,
		size_t n_points,
		size_t n_feats,
		size_t batch_size);

// y
typedef struct _asgd_data_y asgd_data_y_t;
struct _asgd_data_y
{
	bool (*next_block)(asgd_data_y_t *state, uint32_t **data, size_t *rows);
};

typedef struct _asgd_data_y_memory asgd_data_y_memory_t;
struct _asgd_data_y_memory
{
	asgd_data_y_t data;

	uint32_t *items;
	size_t n_points;
	size_t points_left;
	size_t batch_size;
};

void asgd_data_y_memory_init(
		asgd_data_y_memory_t *data,
		uint32_t *items,
		size_t n_points,
		size_t batch_size);

#endif

