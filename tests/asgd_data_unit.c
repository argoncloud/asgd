#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "../asgd_data.h"
#include "test_utils.h"

static bool test_data_plain_arrays()
{

	printf("%s Checking asgd_data_plain_arrays\n", ASGD_TEST_TEST);
	bool succ = true;

	size_t n_classes = 23;
	size_t n_feats = 31;
	size_t n_points = 100;
	size_t batch_size = 13;

	asgd_data_plain_arrays_t data;
	asgd_data_plain_arrays_init(
			&data,
			(float *)30000,
			(float *)70000,
			n_points,
			batch_size);

	float *exp_X[] = {
		(float *)(30000 + 0 * n_feats * batch_size * sizeof(float)),
		(float *)(30000 + 1 * n_feats * batch_size * sizeof(float)),
		(float *)(30000 + 2 * n_feats * batch_size * sizeof(float)),
		(float *)(30000 + 3 * n_feats * batch_size * sizeof(float)),
		(float *)(30000 + 4 * n_feats * batch_size * sizeof(float)),
		(float *)(30000 + 5 * n_feats * batch_size * sizeof(float)),
		(float *)(30000 + 6 * n_feats * batch_size * sizeof(float)),
		(float *)(30000 + 7 * n_feats * batch_size * sizeof(float))
	};
	float *exp_y[] = {
		(float *)(70000 + 0 * batch_size * sizeof(float)),
		(float *)(70000 + 1 * batch_size * sizeof(float)),
		(float *)(70000 + 2 * batch_size * sizeof(float)),
		(float *)(70000 + 3 * batch_size * sizeof(float)),
		(float *)(70000 + 4 * batch_size * sizeof(float)),
		(float *)(70000 + 5 * batch_size * sizeof(float)),
		(float *)(70000 + 6 * batch_size * sizeof(float)),
		(float *)(70000 + 7 * batch_size * sizeof(float))
	};
	size_t exp_n_points[] = {
		13,
		13,
		13,
		13,
		13,
		13,
		13,
		9
	};

	float *get_X;
	float *get_y;
	float *get_margin;
	size_t get_points;
	size_t c = 0;
	while (asgd_data_plain_arrays_get(
				&data,
				n_feats,
				n_classes,
				&get_X, &get_y,
				&get_margin,
				&get_points))
	{
		if (get_X != exp_X[c] || get_y != exp_y[c] || get_points != exp_n_points[c])
		{
			printf("%s Iteration %zu: exp X=%10p y=%10p npoints=%10.10zu\n",
					ASGD_TEST_FAIL, c, exp_X[c], exp_y[c], exp_n_points[c]);
			printf("%s Iteration %zu: got X=%10p y=%10p npoints=%10.10zu\n",
					ASGD_TEST_FAIL, c, get_X, get_y, get_points);
			succ = false;
		}
		++c;
	}

	if (succ)
	{
		printf("%s Completed asgd_data_plain_arrays\n", ASGD_TEST_PASS);
	}
	else
	{
		printf("%s Completed asgd_data_plain_arrays\n", ASGD_TEST_FAIL);
	}
	return succ;
}

int main(void)
{
	bool res = true;
	
	asgd_test_print_header(ASGD_TEST_TEST, "asgd_data_unit");

	res &= test_data_plain_arrays();

	if (res)
	{
		asgd_test_print_footer(ASGD_TEST_PASS, "asgd_data_unit");
		return EXIT_SUCCESS;
	}
	else
	{
		asgd_test_print_footer(ASGD_TEST_FAIL, "asgd_data_unit");
		return EXIT_FAILURE;
	}
}

