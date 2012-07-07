#ifndef _ASGD_DATA_H_
#define _ASGD_DATA_H_

typedef struct _asgd_state_arrays asgd_state_arrays_t;
struct _asgd_state_arrays
{
	long iterations;
	float *X;
	float *y;
};

/**
 * Use the same arrays one or more times
 */
bool asgd_get_arrays(void *state, float **X, float **y);

/**
 * Use the same arrays, but shuffle their rows each time
 */
bool asgd_get_shuffled(void *state, float **X, float **y);

#endif

