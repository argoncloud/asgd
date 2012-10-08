#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "asgd.h"
#include "asgd_data.h"
#include "test_utils.h"

static bool test_asgd_mem()
{
	asgd_test_print_header("asgd_mem");
	bool res = true;

	#undef N_POINTS
	#define N_POINTS 9
	#undef N_CLASSES
	#define N_CLASSES 3
	#undef N_FEATS
	#define N_FEATS 3

	float X_data[N_POINTS][N_FEATS] = {
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.6f, 0.2f, 0.2f},
		{0.2f, 0.6f, 0.2f},
		{0.2f, 0.2f, 0.6f},
		{0.7f, 0.2f, 0.1f},
		{0.2f, 0.7f, 0.1f},
		{0.2f, 0.1f, 0.7f}
	};
	uint32_t y_in_data[N_POINTS] = {0, 1, 2, 0, 1, 2, 0, 1, 2};
	uint32_t y_out_data[N_POINTS] = {0};

	asgd_t *asgd = asgd_init(N_FEATS, N_CLASSES, 1e-1, 1e-1);
	asgd_data_X_memory_t X;
	asgd_data_y_memory_t y_in;
	asgd_data_y_memory_t y_out;

	asgd_data_X_memory_init(&X, (float *)X_data, N_POINTS, N_FEATS, 3);
	asgd_data_y_memory_init(&y_in, (uint32_t *)y_in_data, N_POINTS, 3);

	asgd_fit(asgd, (asgd_data_X_t *)&X, (asgd_data_y_t *)&y_in);

	asgd_data_X_memory_init(&X, (float *)X_data, N_POINTS, N_FEATS, 3);
	asgd_data_y_memory_init(&y_out, (uint32_t *)y_out_data, N_POINTS, 3);

	asgd_predict(asgd, (asgd_data_X_t *)&X, (asgd_data_y_t *)&y_out);

	printf("Comparing expected and received classes\n");
	for (size_t i=0; i < N_POINTS; ++i)
	{
		res = res ? y_in_data[i] == y_out_data[i] : res;
		printf("%u : %u\n", y_in_data[i], y_out_data[i]);
	}

	asgd_destr(asgd);

	asgd_test_print_footer("asgd_mem", res);
	return res;
}

static bool test_asgd_file()
{
	asgd_test_print_header("asgd_file");
	bool res = true;

	size_t n_points = 7500;
	size_t n_feats = 3;
	size_t n_classes = 3;
	size_t batch_size = 15;

	const char *X_file_name = "test_X_data";
	const char *y_in_file_name = "test_y_in_data";
	const char *y_out_file_name = "test_y_out_data";

	// write test files
	int X_file = open(X_file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	int y_in_file = open(y_in_file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	if (X_file == -1)
	{
		fprintf(stderr, "Could not create test X file\n");
		return false;
	}
	if (y_in_file == -1)
	{
		fprintf(stderr, "Could not create test y file\n");
		return false;
	}

	for (size_t i = 0; i < n_points / 3; ++i)
	{
		int val1 = rand()*1.f/RAND_MAX * 0.5f;
		int val2 = rand()*1.f/RAND_MAX;
		float cr[3] = {0.5f+(0.5f-val1), val1*val2, val1*(1.f-val2)};
		float cg[3] = {val1*val2, 0.5f+(0.5f-val1), val1*(1.f-val2)};
		float cb[3] = {val1*val2, val1*(1.f-val2), 0.5f+(0.5f-val1)};

		int write_cr = write(X_file, cr, 3*sizeof(float));
		int write_cg = write(X_file, cg, 3*sizeof(float));
		int write_cb = write(X_file, cb, 3*sizeof(float));
		if (write_cr < 3*sizeof(float)
				|| write_cg < 3*sizeof(float)
				|| write_cb < 3*sizeof(float))
		{
			fprintf(stderr, "Could not write to test X file\n");
			return false;
		}

		uint32_t ur = 0;
		uint32_t ug = 1;
		uint32_t ub = 2;
		int write_r = write(y_in_file, &ur, sizeof(uint32_t));
		int write_g = write(y_in_file, &ug, sizeof(uint32_t));
		int write_b = write(y_in_file, &ub, sizeof(uint32_t));
		if (write_r < sizeof(uint32_t)
				|| write_g < sizeof(uint32_t)
				|| write_b < sizeof(uint32_t))
		{
			fprintf(stderr, "Could not write to test y file\n");
			return false;
		}

	}

	int X_done = close(X_file);
	if (X_done == -1)
	{
		fprintf(stderr, "Could not close test X file\n");
		return false;
	}
	int y_in_done = close(y_in_file);
	if (y_in_done == -1)
	{
		fprintf(stderr, "Could not close test y_in file\n");
		return false;
	}

	int y_out_file = creat(y_out_file_name, S_IRWXU);
	int y_out_done = close(y_out_file);
	if (y_out_done == -1)
	{
		fprintf(stderr, "Could not close test y_out file\n");
		return false;
	}
	truncate(y_out_file_name, n_points * sizeof(uint32_t));

	// start actual test
	asgd_t *asgd = asgd_init(n_feats, n_classes, 1e-1, 1e-1);
	asgd_data_X_file_t X;
	asgd_data_y_file_t y_in;
	asgd_data_y_file_t y_out;

	asgd_data_X_file_init(
			&X,
			X_file_name,
			n_points,
			n_feats,
			batch_size);

	asgd_data_y_file_init(
			&y_in,
			y_in_file_name,
			n_points,
			batch_size,
			false);

	asgd_fit(asgd, (asgd_data_X_t *)&X, (asgd_data_y_t *)&y_in);

	asgd_data_X_file_init(
			&X,
			X_file_name,
			n_points,
			n_feats,
			batch_size);

	asgd_data_y_file_init(
			&y_out,
			y_out_file_name,
			n_points,
			batch_size,
			true);

	asgd_predict(asgd, (asgd_data_X_t *)&X, (asgd_data_y_t *)&y_out);

	asgd_data_y_file_init(
			&y_in,
			y_in_file_name,
			n_points,
			n_points,
			false);

	asgd_data_y_file_init(
			&y_out,
			y_out_file_name,
			n_points,
			n_points,
			false);

	uint32_t *in_data, *out_data;
	size_t rows;
	y_in.data.next_block((asgd_data_y_t *)&y_in, &in_data, &rows);
	y_out.data.next_block((asgd_data_y_t *)&y_out, &out_data, &rows);
	for (size_t i = 0; i < n_points; ++i)
	{
		if (in_data[i] != out_data[i])
		{
			res = false;
			break;
		}
	}

	// remove the files
	unlink(X_file_name);
	unlink(y_in_file_name);
	//unlink(y_out_file_name);

	asgd_test_print_footer("asgd_file", res);
	return res;
}

int main(void)
{
	asgd_test_print_header("asgd_unit");
	bool res = true;

	res &= test_asgd_mem();
	res &= test_asgd_file();

	asgd_test_print_footer("asgd_unit", res);
	return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

