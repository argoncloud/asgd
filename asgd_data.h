#ifndef _ASGD_DATA_H_
#define _ASGD_DATA_H_

#include <stdint.h>

#include "asgd.h"

typedef struct _asgd_data_memory asgd_data_memory_t;
struct _asgd_data_memory
{
	float *X;
	uint32_t *y;
	size_t points_left;
	size_t batch_size;

	float *margin;
};

void asgd_data_memory_init(
		asgd_data_memory_t *data,
		float *X,
		uint32_t *y,
		size_t n_points,
		size_t batch_size);

bool asgd_data_memory_get(
		void *state,
		size_t n_feats,
		size_t n_classes,
		float **X,
		uint32_t **y,
		float **margin,
		size_t *batch_size);

#endif

