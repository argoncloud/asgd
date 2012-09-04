#ifndef _ASGD_DATA_H_
#define _ASGD_DATA_H_

#include "asgd.h"

typedef struct _asgd_data_plain_arrays asgd_data_plain_arrays_t;
struct _asgd_data_plain_arrays
{
	float *X;
	float *y;
	size_t points_left;
	size_t batch_size;

	float *margin;
};

void asgd_data_plain_arrays_init(
		asgd_data_plain_arrays_t *data,
		float *X,
		float *y,
		size_t n_points,
		size_t batch_size);

bool asgd_data_plain_arrays_get(
		void *state,
		size_t n_feats,
		size_t n_classes,
		float **X,
		float **y,
		float **margin,
		size_t *batch_size);

#endif

