#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_utils.h"

void asgd_test_print_header(
		const char *name,
		size_t *depth)
{
	for (size_t d=*depth; d > 0; --d) printf("\t");
	printf("%s Starting test *** %s ***\n", ASGD_TEST_TEST, name);
	++*depth;
}

void asgd_test_print_footer(
		const char *name,
		bool status,
		size_t *depth)
{
	if (*depth > 0)
	{
		--*depth;
	}
	const char *status_string = status ? ASGD_TEST_PASS : ASGD_TEST_FAIL;
	for (size_t d=*depth; d > 0; --d) printf("\t");
	printf("%s Completed test *** %s ***\n", status_string, name);
}

static void asgd_test_print_matrix_header(
		const char *status,
		const char *name,
		size_t *depth)
{
	for (size_t d=*depth; d > 0; --d) printf("\t");
	printf("%s Matrix %s not as expected. Comparison follows:\n", status, name);
}

static void asgd_test_print_row(
		const char *status,
		const float *row,
		size_t len,
		size_t *depth)
{
	const int float_str_field = 12;
	const int float_str_prec = 5;

	for (size_t d=*depth; d > 0; --d) printf("\t");
	printf("%s ", status);
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
		float slack,
		size_t *depth)
{
	bool same = true;

	for (size_t d=*depth; d > 0; --d) printf("\t");
	printf("%s Checking matrix %s\n", ASGD_TEST_TEST, name);
	for (size_t r = 0; r < rows; ++r)
	{
		bool print_row = false;
		for (size_t c = 0; c < cols; ++c)
		{
			if (fabs(exp[r*cols+c] - got[r*cols+c]) > slack)
			{
				print_row = true;
				if (same)
				{
					same = false;
					asgd_test_print_matrix_header(ASGD_TEST_FAIL, name, depth);
				}
			}
		}
		if (print_row)
		{
			for (size_t d=*depth; d > 0; --d) printf("\t");
			printf("%s Expected row %zu:\n", ASGD_TEST_FAIL, r);
			asgd_test_print_row(ASGD_TEST_FAIL, exp + r*cols, cols, depth);
			for (size_t d=*depth; d > 0; --d) printf("\t");
			printf("%s Found row %zu:\n", ASGD_TEST_FAIL, r);
			asgd_test_print_row(ASGD_TEST_FAIL, got + r*cols, cols, depth);
		}
	}

	if (same)
	{
		for (size_t d=*depth; d > 0; --d) printf("\t");
		printf("%s Matrix %s as expected\n", ASGD_TEST_PASS, name);
	}
	else
	{
		for (size_t d=*depth; d > 0; --d) printf("\t");
		printf("%s Matrix %s not as expected\n", ASGD_TEST_FAIL, name);
	}
	return same;
}

