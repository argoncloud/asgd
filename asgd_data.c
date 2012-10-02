#include "asgd_data.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "asgd_errors.h"

void asgd_data_buffer_init(asgd_data_buffer_t *buffer)
{
	buffer->data = NULL;
}

float *asgd_data_buffer_get(asgd_data_buffer_t *buffer, size_t size)
{
	buffer->data = realloc(buffer->data, size);
	asgd_assert(buffer->data != NULL, ASGD_ERROR_MARGIN_NOMEM);
	return buffer->data;
}

void asgd_data_buffer_destr(asgd_data_buffer_t *buffer)
{
	free(buffer->data);
	buffer->data = NULL;
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

static bool asgd_data_X_file_next_block(
		asgd_data_X_t *state,
		float **data,
		size_t *rows)
{
	asgd_data_X_file_t *fstate = (asgd_data_X_file_t *)state;

	if (fstate->points_left > 0)
	{
		// open the file
		if (fstate->fd == -1)
		{
			fstate->fd = open(fstate->file_name, O_RDONLY);
			asgd_assert(fstate->fd != -1, ASGD_ERROR_CANNOT_OPEN_X_FILE);
		}

		// release previously mapped pages
		if (fstate->map_data != NULL)
		{
			asgd_assert(
					munmap(fstate->map_data, fstate->map_len) != -1,
					ASGD_ERROR_CANNOT_MUNMAP_REGION);
			fstate->map_data = NULL;
		}

		size_t block_size = fstate->batch_size < fstate->points_left ?
			fstate->batch_size : fstate->points_left;

		size_t data_len = block_size * fstate->n_feats * sizeof(**data);
		off_t data_start = fstate->points_done * fstate->n_feats * sizeof(**data);
		off_t page_start = data_start / fstate->page_size * fstate->page_size;
		off_t start_slack = data_start - page_start;

		fstate->map_len = start_slack + data_len;
		fstate->map_data = mmap(
				0,
				fstate->map_len,
				PROT_READ,
				MAP_PRIVATE,
				fstate->fd,
				page_start);

		asgd_assert(fstate->map_data != MAP_FAILED, ASGD_ERROR_CANNOT_MMAP_REGION);

		*data = (float *)(fstate->map_data + start_slack);
		*rows = block_size;

		fstate->points_done += block_size;
		fstate->points_left -= block_size;

		return true;
	}
	else
	{
		if (fstate->fd != -1)
		{
			int close_stat = close(fstate->fd);
			asgd_assert(close_stat != -1, ASGD_ERROR_CANNOT_CLOSE_X_FILE);
			fstate->fd = -1;
		}
		return false;
	}
}

void asgd_data_X_file_init(
		asgd_data_X_file_t *data,
		const char *file_name,
		size_t n_points,
		size_t n_feats,
		size_t batch_size)
{
	data->data.next_block = asgd_data_X_file_next_block;

	data->file_name = file_name;
	data->fd = -1;
	data->page_size = sysconf(_SC_PAGESIZE);
	asgd_assert(data->page_size != -1, ASGD_ERROR_CANNOT_RETRIEVE_PAGESIZE);
	data->map_data = NULL;
	data->map_len = 0;

	data->n_points = n_points;
	data->n_feats = n_feats;
	data->points_done = 0;
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

