#include "asgd_data.h"

bool asgd_get_arrays(void *state, float **X, float **y)
{
	asgd_state_arrays_t *arrays_state = (asgd_state_arrays_t *)state;
	
	*X = arrays_state->X;
	*y = arrays_state->y;
	--arrays_state->iterations;
	return arrays_state->iterations >= 0;
}

bool asgd_get_shuffled(void *state, float **X, float **y)
{
	asgd_state_shuffled_t *shuffled_state = (asgd_state_shuffled_t *)state;
	
	/**X = arrays_state->X;
	*y = arrays_state->y;
	--arrays_state->iterations;
	return arrays_state->iterations >= 0;*/
}

