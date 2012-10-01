#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "asgd.h"
#include "asgd_data.h"
#include "test_utils.h"

bool test_asgd()
{
	asgd_test_print_header("asgd");
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

	asgd_test_print_footer("asgd", res);
	return res;
}

int main(void)
{
	asgd_test_print_header("asgd_unit");
	bool res = true;

	res &= test_asgd();

	asgd_test_print_footer("asgd_unit", res);
	return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

