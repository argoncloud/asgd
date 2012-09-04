#include "asgd_data.h"

#include <stdlib.h>

#include "asgd_errors.h"

void asgd_data_plain_arrays_init(
		asgd_data_plain_arrays_t *data,
		float *X,
		float *y,
		size_t n_points,
		size_t batch_size)
{
	data->X = X;
	data->y = y;
	data->points_left = n_points;
	data->batch_size = batch_size;
	data->margin = NULL;
}

bool asgd_data_plain_arrays_get(
		void *state,
		size_t n_feats,
		size_t n_classes,
		float **X,
		float **y,
		float **margin,
		size_t *batch_size)
{
	asgd_data_plain_arrays_t *cstate = (asgd_data_plain_arrays_t *)state;
	
	if (cstate->points_left == 0)
	{
		free(cstate->margin);
		return false;
	}
	else
	{
		size_t margin_size =
			cstate->batch_size * n_classes * sizeof(*(cstate->margin));
		cstate->margin = realloc(cstate->margin, margin_size);
		asgd_assert(cstate->margin != NULL, ASGD_ERROR_DATA_NOMEM);

		size_t curr_batch_size = cstate->batch_size < cstate->points_left ?
			cstate->batch_size : cstate->points_left;

		*X = cstate->X;
		*y = cstate->y;
		cstate->X += cstate->batch_size * n_feats;
		cstate->y += cstate->batch_size;
		*margin = cstate->margin;
		*batch_size = curr_batch_size;
		cstate->points_left -= curr_batch_size;
		
		return true;
	}
}

