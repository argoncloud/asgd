#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../asgd_core.h"
#include "../asgd_errors.h"
#include "test_utils.h"

static bool test_core_partial_fit()
{
	asgd_test_print_header("core_partial_fit");

	unsigned long n_observs = 0;
	float sgd_step_size = 1e-1f;
	float asgd_step_size = 0.5f;

	const float l2_reg = 1e-3f;
	const float sgd_step_size0 = 1e-1f;
	const float sgd_step_size_sched_exp = 2.f / 3.f;
	const float sgd_step_size_sched_mul = l2_reg;

	#undef N_POINTS
	#undef N_FEATS
	#undef N_CLASSES
	#define N_POINTS (5)
	#define N_FEATS (5)
	#define N_CLASSES (5)

	float sgd_weights[N_FEATS][N_CLASSES] = {
		{1.f, 0.f, 0.f, 0.f, 0.f},
		{0.f, 1.f, 0.f, 0.f, 0.f},
		{0.f, 0.f, 1.f, 0.f, 0.f},
		{0.f, 0.f, 0.f, 1.f, 0.f},
		{0.f, 0.f, 0.f, 0.f, 1.f}
	};
	float sgd_bias[N_CLASSES] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
	float asgd_weights[N_FEATS][N_CLASSES] = {
		{2.f, 4.f, 6.f, 8.f, 10.f},
		{2.f, 4.f, 6.f, 8.f, 10.f},
		{2.f, 4.f, 6.f, 8.f, 10.f},
		{2.f, 4.f, 6.f, 8.f, 10.f},
		{2.f, 4.f, 6.f, 8.f, 10.f}
	};
	float asgd_bias[N_CLASSES] = {2.f, 4.f, 6.f, 8.f, 10.f};

	float X[N_POINTS][N_FEATS] = {
		{1.f, 0.1f, 0.1f, 0.1f, 0.1f},
		{0.1f, 1.f, 0.1f, 0.1f, 0.1f},
		{0.1f, 0.1f, 1.f, 0.1f, 0.1f},
		{0.1f, 0.1f, 0.1f, 1.f, 0.1f},
		{0.1f, 0.1f, 0.1f, 0.1f, 1.f}
	};
	uint32_t y[N_POINTS] = {0,1,2,3,4};

	float *margin = malloc(N_POINTS * N_CLASSES * sizeof(*margin));
	asgd_assert(margin != NULL, ASGD_ERROR_MARGIN_NOMEM);

	unsigned long exp_observs = 1;
	float exp_sgd_step_size = 0.099993334;
	float exp_asgd_step_size = 1.f;
	float exp_sgd_weights[N_FEATS][N_CLASSES] = {
		{0.9919f, -0.026f, -0.026f, -0.026f, -0.026f},
		{-0.026f, 0.9919f, -0.026f, -0.026f, -0.026f},
		{-0.026f, -0.026f, 0.9919f, -0.026f, -0.026f},
		{-0.026f, -0.026f, -0.026f, 0.9919f, -0.026f},
		{-0.026f, -0.026f, -0.026f, -0.026f, 0.9919f}
	};
	float exp_sgd_bias[N_CLASSES] = {0.02f, 0.02f, 0.02f, 0.02f, 0.02f};

	float exp_asgd_weights[N_FEATS][N_CLASSES] = {
		{1.49595f, 1.987f, 2.987f, 3.987f, 4.987f},
		{0.987f, 2.49595f, 2.987f, 3.987f, 4.987f},
		{0.987f, 1.987f, 3.49595f, 3.987f, 4.987f},
		{0.987f, 1.987f, 2.987f, 4.49595f, 4.987f},
		{0.987f, 1.987f, 2.987f, 3.987f, 5.49595f}
	};
	float exp_asgd_bias[N_CLASSES] = {1.01f, 2.01f, 3.01f, 4.01f, 5.01f};

	asgd_core_partial_fit(
			&n_observs,
			&sgd_step_size,
			&asgd_step_size,

			l2_reg,
			sgd_step_size0,
			sgd_step_size_sched_exp,
			sgd_step_size_sched_mul,

			N_POINTS,
			N_FEATS,
			N_CLASSES,

			(float *)sgd_weights,
			(float *)sgd_bias,
			(float *)asgd_weights,
			(float *)asgd_bias,

			(float *)X,
			(uint32_t *)y,

			margin);

	bool res = true;

	res &= asgd_test_matrix_diff(
			"sgd_weights",
			(float *)exp_sgd_weights,
			(float *)sgd_weights,
			N_FEATS,
			N_CLASSES,
			1e-5f);

	res &= asgd_test_matrix_diff(
			"sgd_bias",
			(float *)exp_sgd_bias,
			(float *)sgd_bias,
			1,
			N_CLASSES,
			1e-5f);

	res &= asgd_test_matrix_diff(
			"asgd_weights",
			(float *)exp_asgd_weights,
			(float *)asgd_weights,
			N_FEATS,
			N_CLASSES,
			1e-5f);

	res &= asgd_test_matrix_diff(
			"asgd_bias",
			(float *)exp_asgd_bias,
			(float *)asgd_bias,
			1,
			N_CLASSES,
			1e-5f);

	printf("%s Testing parameters\n", ASGD_TEST_TEST);

	if (n_observs == exp_observs &&
			sgd_step_size == exp_sgd_step_size &&
			asgd_step_size == exp_asgd_step_size)
	{
		printf("%s Parameters as expected\n", ASGD_TEST_PASS);
	}
	else
	{
		res = false;
		printf("%s Parameters not as expected\n", ASGD_TEST_FAIL);
		printf("%s Exp: n_observs=%10lu sgd_step_size=%10f asgd_step_size=%10f\n",
				ASGD_TEST_FAIL,
				exp_observs, exp_sgd_step_size, exp_asgd_step_size);
		printf("%s Got: n_observs=%10lu sgd_step_size=%10f asgd_step_size=%10f\n",
				ASGD_TEST_FAIL,
				n_observs, sgd_step_size, asgd_step_size);
	}

	free(margin);

	asgd_test_print_footer("core_partial_fit", res);
	return res;
}

static bool test_core_decision_function()
{
	bool res = true;
	asgd_test_print_header("core_decision_function");

	#undef N_POINTS
	#undef N_FEATS
	#undef N_CLASSES
	#define N_POINTS (3)
	#define N_FEATS (5)
	#define N_CLASSES (4)

	float sgd_weights[N_FEATS][N_CLASSES] = {
		{10.f, 20.f, 30.f, 40.f},
		{11.f, 21.f, 31.f, 41.f},
		{12.f, 22.f, 32.f, 42.f},
		{13.f, 23.f, 33.f, 43.f},
		{14.f, 24.f, 34.f, 44.f}
	};
	float sgd_bias[N_CLASSES] = {10.f, -10.f, 10.f, -10.f};
	float X[N_POINTS][N_FEATS] = {
		{11.f, 22.f, 33.f, 44.f, 55.f},
		{66.f, 77.f, 88.f, 99.f, 88.f},
		{77.f, 66.f, 55.f, 44.f, 33.f}
	};

	float dec[N_POINTS][N_CLASSES];

	float exp_dec[N_POINTS][N_CLASSES] = {
		{2100.f, 3730.f, 5400.f, 7030.f},
		{5092.f, 9252.f, 13452.f, 17612.f},
		{3200.f, 5930.f, 8700.f, 11430.f}
	};

	core_decision_function(
			N_POINTS,
			N_FEATS,
			N_CLASSES,

			(float *)sgd_weights,
			(float *)sgd_bias,
			(float *)X,

			(float *)dec);

	res &= asgd_test_matrix_diff(
			"dec",
			(float *)exp_dec,
			(float *)dec,
			N_POINTS,
			N_CLASSES,
			1e-5f);

	asgd_test_print_footer("core_decision_function", res);
	return res;
}

static bool test_core_predict()
{
	bool res = true;
	asgd_test_print_header("core_predict");

	#undef N_POINTS
	#undef N_FEATS
	#define N_POINTS (3)
	#define N_CLASSES (4)

	float dec[N_POINTS][N_CLASSES] = {
		{3.f, 5.f, 9.f, 7.f},
		{8.f, 6.f, 4.f, 2.f},
		{1.f, 3.f, 5.f, 7.f}
	};
	uint32_t resv[N_POINTS] = {0, 0, 0};

	uint32_t exp_resv[N_POINTS] = {2, 0, 3};

	core_predict(
			N_POINTS,
			N_CLASSES,

			(float *)dec,
			(uint32_t *)resv);

	printf("%s Checking vector %s\n", ASGD_TEST_TEST, "res");
	for (size_t i = 0; i < N_POINTS; ++i)
	{
		if (exp_resv[i] != resv[i])
		{
			res = false;
		}
	}
	if (!res)
	{
		printf("Exp:\n");
		for (size_t i = 0; i < N_POINTS; ++i)
		{
			printf("%.5d ", exp_resv[i]);
		}
		printf("\nGot:\n");
		for (size_t i = 0; i < N_POINTS; ++i)
		{
			printf("%.5d ", resv[i]);
		}
		printf("\n%s Vector %s not as expected\n", ASGD_TEST_FAIL, "res");
	}
	else
	{
		printf("%s Vector %s as expected\n", ASGD_TEST_PASS, "res");
	}

	if (res)
	{
		asgd_test_print_footer(ASGD_TEST_PASS, "core_predict");
	}
	else
	{
		asgd_test_print_footer(ASGD_TEST_FAIL, "core_predict");
	}

	return res;
}

int main(void)
{
	bool res = true;

	asgd_test_print_header("asgd_data_unit");

	res &= test_core_partial_fit();
	res &= test_core_decision_function();
	res &= test_core_predict();

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

