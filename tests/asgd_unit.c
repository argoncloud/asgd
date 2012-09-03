#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../asgd.h"
#include "../asgd_core.h"
#include "../asgd_data.h"

bool test_get_arrays()
{
	asgd_state_arrays_t state;
	state.X = &state;
	state.y = &state + 0x01;
	state.n_points = 15;
	state.done = false;

	float *X = NULL;
	float *y = NULL;
	float *margin = NULL;
	size_t n_points = 0;
	bool exec = false;

	exec = asgd_get_arrays(asgd, state, &X, &y, &margin, &n_points);
	assert(done == true);
	

	exec = asgd_get_arrays(asgd, state, &X, &y, &margin, &n_points);
	assert(done == false);
}

int main(void)
{

}

