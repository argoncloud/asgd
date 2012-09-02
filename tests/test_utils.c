#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_utils.h"

static void asgd_test_print_row(
		const float *row,
		size_t len)
{
	const int float_str_field = 12;
	const int float_str_prec = 5;
	
	printf("[FAIL] ");
	for (size_t c = 0; c < len; ++c)
	{
		printf("%*.*f",
				float_str_field,
				float_str_prec,
				row[c]);
	}
	printf("\n");
}

bool asgd_test_matrix_diff(
		const char *name,
		const float *exp,
		const float *got,
		size_t rows,
		size_t cols,
		float slack)
{
	bool same = true;
	printf("[FAIL]: Matrix %s not as expected. Comparison follows:\n", name);

	for (size_t r = 0; r < rows; ++r)
	{
		bool print_row = false;
		for (size_t c = 0; c < cols; ++c)
		{
			if (fabs(exp[r*cols+c] - got[r*cols+c]) > slack)
			{
				print_row = true;
				same = false;
			}
		}
		if (print_row)
		{
			printf("[FAIL] Expected row %zu:\n", r);
			asgd_test_print_row(exp + r*cols, cols);
			printf("[FAIL] Found row %zu:\n", r);
			asgd_test_print_row(got + r*cols, cols);
		}
	}

	printf("\n");
	return same;
}

