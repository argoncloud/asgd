#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "asgd_data.h"
#include "test_utils.h"

static bool test_data_buffer()
{
	asgd_test_print_header("asgd_data_buffer");
	bool succ = true;
	asgd_data_buffer_t buffer;

	asgd_data_buffer_init(&buffer);
	for (size_t i = 1; i < 100; ++i)
	{
		float *data = asgd_data_buffer_get(&buffer, i*1024*sizeof(*data));
		data[i*1024-1] = 123.45f;
	}
	asgd_data_buffer_destr(&buffer);

	asgd_test_print_footer("asgd_data_buffer", succ);
	return succ;
}

static bool test_data_X_memory()
{
	asgd_test_print_header("asgd_data_X_memory");
	bool succ = true;

	size_t n_feats = 31;
	size_t n_points = 100;
	size_t batch_size = 13;

	asgd_data_X_memory_t X;
	asgd_data_X_memory_init(
			&X,
			(float *)30000,
			n_points,
			n_feats,
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
	size_t exp_rows[] = {13, 13, 13, 13, 13, 13, 13, 9};

	float *get_X;
	size_t get_rows;
	size_t c = 0;
	while (X.data.next_block(
				(asgd_data_X_t *)&X,
				&get_X,
				&get_rows))
	{
		if (get_X != exp_X[c] || get_rows != exp_rows[c])
		{
			printf("%s Iteration %zu: exp X=%10p rows=%10.10zu\n",
					ASGD_TEST_FAIL, c, exp_X[c], exp_rows[c]);
			printf("%s Iteration %zu: got X=%10p rows=%10.10zu\n",
					ASGD_TEST_FAIL, c, get_X, get_rows);
			succ = false;
		}
		++c;
	}

	asgd_test_print_footer("asgd_data_X_memory", succ);
	return succ;
}

static bool test_data_y_memory()
{
	asgd_test_print_header("asgd_data_X_memory");
	bool succ = true;

	size_t n_points = 100;
	size_t batch_size = 13;

	asgd_data_y_memory_t y;
	asgd_data_y_memory_init(
			&y,
			(uint32_t *)70000,
			n_points,
			batch_size);

	uint32_t *exp_y[] = {
		(uint32_t *)(70000 + 0 * batch_size * sizeof(uint32_t)),
		(uint32_t *)(70000 + 1 * batch_size * sizeof(uint32_t)),
		(uint32_t *)(70000 + 2 * batch_size * sizeof(uint32_t)),
		(uint32_t *)(70000 + 3 * batch_size * sizeof(uint32_t)),
		(uint32_t *)(70000 + 4 * batch_size * sizeof(uint32_t)),
		(uint32_t *)(70000 + 5 * batch_size * sizeof(uint32_t)),
		(uint32_t *)(70000 + 6 * batch_size * sizeof(uint32_t)),
		(uint32_t *)(70000 + 7 * batch_size * sizeof(uint32_t))
	};
	size_t exp_rows[] = {13, 13, 13, 13, 13, 13, 13, 9};

	uint32_t *get_y;
	size_t get_rows;
	size_t c = 0;
	while (y.data.next_block(
				(asgd_data_y_t *)&y,
				&get_y,
				&get_rows))
	{
		if (get_y != exp_y[c] || get_rows != exp_rows[c])
		{
			printf("%s Iteration %zu: exp y=%10p rows=%10.10zu\n",
					ASGD_TEST_FAIL, c, exp_y[c], exp_rows[c]);
			printf("%s Iteration %zu: got y=%10p rows=%10.10zu\n",
					ASGD_TEST_FAIL, c, get_y, get_rows);
			succ = false;
		}
		++c;
	}

	asgd_test_print_footer("asgd_data_y_memory", succ);
	return succ;
}

int main(void)
{
	bool res = true;

	asgd_test_print_header("asgd_data_unit");

	res &= test_data_buffer();
	res &= test_data_X_memory();
	res &= test_data_y_memory();

	asgd_test_print_footer("asgd_data_unit", res);
	return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

