#include "asgd_data.h"

#include <stdlib.h>

#include "asgd_errors.h"

void asgd_data_buffer_init(asgd_data_buffer_t *buffer)
{
	buffer->buffer = NULL;
}

float *asgd_data_buffer_get(asgd_data_buffer_t *buffer, size_t size)
{
	buffer->buffer = realloc(buffer->buffer, size);
	asgd_assert(buffer->buffer != NULL, ASGD_ERROR_MARGIN_NOMEM);
	return buffer->buffer;
}

void asgd_data_buffer_destr(asgd_data_buffer_t *buffer)
{
	free(buffer->buffer);
}

static bool asgd_data_X_memory_next_block(
		asgd_data_X_t *state,
		float **data,
		size_t *rows)
{
	asgd_data_X_memory_t *mstate = (asgd_data_X_memory_t *)state;

	if (mstate->points_left > 0)
	{
		size_t block_size = mstate->batch_size < mstate->points_left ?
			mstate->batch_size : mstate->points_left;
		mstate->points_left -= block_size;

		*data = mstate->items;
		*rows = block_size;

		mstate->items += block_size * mstate->n_feats;
		return true;
	}
	else
	{
		return false;
	}
}

void asgd_data_X_memory_init(
		asgd_data_X_memory_t *data,
		float *items,
		size_t n_points,
		size_t n_feats,
		size_t batch_size)
{
	data->data.next_block = asgd_data_X_memory_next_block;

	data->items = items;
	data->n_points = n_points;
	data->n_feats = n_feats;
	data->points_left = n_points;
	data->batch_size = batch_size;
}

static bool asgd_data_y_memory_next_block(
		asgd_data_y_t *state,
		uint32_t **data,
		size_t *rows)
{
	asgd_data_y_memory_t *mstate = (asgd_data_y_memory_t *)state;

	if (mstate->points_left > 0)
	{
		size_t block_size = mstate->batch_size < mstate->points_left ?
			mstate->batch_size : mstate->points_left;
		mstate->points_left -= block_size;

		*data = mstate->items;
		*rows = block_size;

		mstate->items += block_size;
		return true;
	}
	else
	{
		return false;
	}
}

void asgd_data_y_memory_init(
		asgd_data_y_memory_t *data,
		uint32_t *items,
		size_t n_points,
		size_t batch_size)
{
	data->data.next_block = asgd_data_y_memory_next_block;

	data->items = items;
	data->n_points = n_points;
	data->points_left = n_points;
	data->batch_size = batch_size;
}

