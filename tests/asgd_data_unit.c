#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "asgd_data.h"
#include "asgd_errors.h"
#include "test_utils.h"

static bool test_data_buffer(size_t *depth)
{
	asgd_test_print_header("asgd_data_buffer", depth);
	bool succ = true;
	asgd_data_buffer_t buffer;

	asgd_assert(asgd_data_buffer_init(&buffer), ASGD_ERROR_BUFFER_INIT);
	for (size_t i = 1; i < 100; ++i)
	{
		asgd_assert(
				asgd_data_buffer_get(&buffer, i*1024*sizeof(*buffer.data)),
				ASGD_ERROR_BUFFER_GET);
		buffer.data[i*1024-1] = 123.45f;
	}
	asgd_assert(asgd_data_buffer_destr(&buffer), ASGD_ERROR_BUFFER_DESTR);

	asgd_test_print_footer("asgd_data_buffer", succ, depth);
	return succ;
}

static bool test_data_X_memory(size_t * depth)
{
	asgd_test_print_header("asgd_data_X_memory", depth);
	bool succ = true;

	size_t n_feats = 31;
	size_t n_points = 100;
	size_t batch_size = 13;

	asgd_data_X_memory_t X;
	asgd_assert(asgd_data_X_memory_init(
			&X,
			(float *)30000,
			n_points,
			n_feats,
			batch_size), ASGD_ERROR_DATA_X_INIT);

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
	while (X.data.next_block((asgd_data_X_t *)&X, &get_X, &get_rows) && get_rows)
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
	} while(get_rows);

	asgd_test_print_footer("asgd_data_X_memory", succ, depth);
	return succ;
}

static bool test_data_y_memory(size_t *depth)
{
	asgd_test_print_header("asgd_data_X_memory", depth);
	bool succ = true;

	size_t n_points = 100;
	size_t batch_size = 13;

	asgd_data_y_memory_t y;
	asgd_assert(asgd_data_y_memory_init(
			&y,
			(uint32_t *)70000,
			n_points,
			batch_size), ASGD_ERROR_DATA_Y_INIT);

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
	while (y.data.next_block((asgd_data_y_t *)&y, &get_y, &get_rows) && get_rows)
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

	asgd_test_print_footer("asgd_data_y_memory", succ, depth);
	return succ;
}

static bool test_data_X_file(size_t *depth)
{
	asgd_test_print_header("asgd_data_X_file", depth);
	bool res = true;

	size_t n_feats = 53;
	size_t n_points = 10000;
	size_t batch_size = 1800;

	const char *file_name = "test_X_data";

	// write test file
	int file = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	if (file == -1)
	{
		fprintf(stderr, "Could not create test file\n");
		return false;
	}

	for (size_t i = 0; i < n_points * n_feats; ++i)
	{
		float num = i;
		int write_state = write(file, &num, sizeof(num));
		if (write_state < sizeof(num))
		{
			fprintf(stderr, "Could not write to test file\n");
			return false;
		}
	}

	int done = close(file);
	if (done == -1)
	{
		fprintf(stderr, "Could not close test file\n");
		return false;
	}

	// start actual test
	asgd_data_X_file_t X;
	asgd_assert(asgd_data_X_file_init(
			&X,
			file_name,
			n_points,
			n_feats,
			batch_size), ASGD_ERROR_DATA_X_INIT);

	float *data;
	size_t rows;
	bool found = false;
	while (X.data.next_block((asgd_data_X_t *)&X, &data, &rows) && rows)
	{
		if (found)
		{
			res = false;
			break;
		}
		if (data[rows * n_feats - 1] == n_points * n_feats - 1)
		{
			found = true;
		}
	}
	res &= found;

	// remove the file
	unlink(file_name);

	asgd_test_print_footer("asgd_data_X_file", res, depth);
	return res;
}

static bool test_data_y_file(size_t *depth)
{
	asgd_test_print_header("asgd_data_y_file", depth);
	bool res = true;

	size_t n_points = 10000;
	size_t batch_size = 1800;

	const char *file_name = "test_y_data";

	// write test file
	int file = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	if (file == -1)
	{
		fprintf(stderr, "Could not create test file\n");
		return false;
	}

	for (size_t i = 0; i < n_points; ++i)
	{
		uint32_t num = i;
		int write_state = write(file, &num, sizeof(num));
		if (write_state < sizeof(num))
		{
			fprintf(stderr, "Could not write to test file\n");
			return false;
		}
	}

	int done = close(file);
	if (done == -1)
	{
		fprintf(stderr, "Could not close test file\n");
		return false;
	}

	// start actual test
	asgd_data_y_file_t y;
	asgd_assert(asgd_data_y_file_init(
			&y,
			file_name,
			n_points,
			batch_size,
			true), ASGD_ERROR_DATA_Y_INIT);

	uint32_t *data;
	size_t rows;
	bool found = false;
	while (y.data.next_block((asgd_data_y_t *)&y, &data, &rows) && rows)
	{
		if (found)
		{
			res = false;
			break;
		}
		if (data[rows - 1] == n_points - 1)
		{
			found = true;
		}
	}
	res &= found;

	// remove the file
	unlink(file_name);

	asgd_test_print_footer("asgd_data_y_file", res, depth);
	return res;
}

int main(void)
{
	size_t depth = 0;
	bool res = true;

	asgd_test_print_header("asgd_data_unit", &depth);

	res &= test_data_buffer(&depth);
	res &= test_data_X_memory(&depth);
	res &= test_data_y_memory(&depth);
	res &= test_data_X_file(&depth);
	res &= test_data_y_file(&depth);

	asgd_test_print_footer("asgd_data_unit", res, &depth);
	return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

